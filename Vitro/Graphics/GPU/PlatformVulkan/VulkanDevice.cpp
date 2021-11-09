module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <array>
#include <bit>
#include <memory>
#include <string_view>
#include <vector>
export module vt.Graphics.Vulkan.Device;

import vt.Core.Array;
import vt.Core.SmallList;
import vt.Graphics.DeviceBase;
import vt.Graphics.Handle;
import vt.Graphics.RingBuffer;
import vt.Graphics.Vulkan.DescriptorPool;
import vt.Graphics.Vulkan.Handle;
import vt.Graphics.Vulkan.SyncTokenPool;

namespace vt::vulkan
{
	export class VulkanDevice final : public DeviceBase
	{
	public:
		VulkanDevice(Adapter const& in_adapter) :
			adapter(in_adapter.vulkan),
			queue_families(query_queue_families()),
			device(make_device()),
			api(std::make_unique<DeviceApiTable>(InstanceApiTable::get().vkGetDeviceProcAddr, adapter, device.get())),
			sync_tokens(*api),
			descriptor_pool(*api)
		{
			api->vkGetDeviceQueue(device.get(), queue_families.render, 0, &render_queue);
			api->vkGetDeviceQueue(device.get(), queue_families.compute, 0, &compute_queue);
			api->vkGetDeviceQueue(device.get(), queue_families.copy, 0, &copy_queue);

			initialize_allocator();
		}

		CopyCommandList make_copy_command_list() override
		{
			return VulkanCommandList<CommandType::Copy>(queue_families.copy, *api);
		}

		ComputeCommandList make_compute_command_list() override
		{
			return VulkanCommandList<CommandType::Compute>(queue_families.compute, *api);
		}

		RenderCommandList make_render_command_list() override
		{
			return VulkanCommandList<CommandType::Render>(queue_families.render, *api);
		}

		Buffer make_buffer(BufferSpecification const& spec) override
		{
			return {VulkanBuffer(spec, *api, allocator.get()), spec};
		}

		Image make_image(ImageSpecification const& spec) override
		{
			return {VulkanImage(spec, *api, allocator.get()), spec};
		}

		SmallList<ComputePipeline> make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs) override
		{
			std::vector<VkComputePipelineCreateInfo> pipeline_infos;
			pipeline_infos.reserve(specs.size());
			for(auto const& spec : specs)
				pipeline_infos.emplace_back(convert_compute_pipeline_spec(spec));

			Array<VkPipeline> pipelines(specs.size());

			auto result = api->vkCreateComputePipelines(api->device, pipeline_cache.get(), count(pipeline_infos),
														pipeline_infos.data(), nullptr, pipelines.data());

			std::vector<ComputePipeline> compute_pipelines;
			compute_pipelines.reserve(specs.size());
			for(auto pipeline : pipelines)
				compute_pipelines.emplace_back(VulkanComputePipeline(pipeline, *api));

			VT_CHECK_RESULT(result, "Failed to create Vulkan graphics pipelines.");
			return compute_pipelines;
		}

		std::vector<RenderPipeline> make_render_pipelines(ArrayView<RenderPipelineSpecification> specs) override
		{
			std::vector<GraphicsPipelineInfoState> states;
			states.reserve(specs.size());
			for(auto& spec : specs)
				states.emplace_back(spec);

			std::vector<VkGraphicsPipelineCreateInfo> pipeline_infos;
			pipeline_infos.reserve(specs.size());
			for(auto const& state : states)
				pipeline_infos.emplace_back(state.convert());

			Array<VkPipeline> pipelines(specs.size());

			auto result = api->vkCreateGraphicsPipelines(device.get(), pipeline_cache.get(), count(pipeline_infos),
														 pipeline_infos.data(), nullptr, pipelines.data());

			std::vector<RenderPipeline> render_pipelines;
			render_pipelines.reserve(specs.size());
			for(auto pipeline : pipelines)
				render_pipelines.emplace_back(VulkanRenderPipeline(pipeline, *api));

			VT_CHECK_RESULT(result, "Failed to create Vulkan graphics pipelines.");
			return render_pipelines;
		}

		std::vector<DescriptorSet> make_descriptor_sets(ArrayView<DescriptorSetLayout> layouts) override
		{
			return descriptor_pool.make_descriptor_sets(layouts, *api);
		}

		DescriptorSetLayout make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec) override
		{
			return VulkanDescriptorSetLayout(spec, *api);
		}

		RenderPass make_render_pass(RenderPassSpecification const& spec) override
		{
			return VulkanRenderPass(spec, *api);
		}

		RootSignature make_root_signature(RootSignatureSpecification const& spec) override
		{
			return VulkanRootSignature(spec, *api);
		}

		Sampler make_sampler(SamplerSpecification const& spec) override
		{
			return VulkanSampler(spec, *api);
		}

		Shader make_shader(char const path[]) override
		{
			return VulkanShader(path, *api);
		}

		SwapChain make_swap_chain(Window& window, uint8_t buffer_count = SwapChain::DEFAULT_BUFFERS) override
		{
			return VulkanSwapChain(queue_families.render, window, buffer_count, sync_tokens, *api);
		}

		void update_descriptors(ArrayView<DescriptorUpdate>)
		{}

		void* map(Buffer const& buffer) override
		{
			return map_resource(buffer);
		}

		void* map(Image const& image) override
		{
			return map_resource(image);
		}

		void unmap(Buffer const& buffer) override
		{
			vmaUnmapMemory(allocator.get(), buffer.vulkan.get_allocation());
		}

		void unmap(Image const& image) override
		{
			vmaUnmapMemory(allocator.get(), image.vulkan.get_allocation());
		}

		SyncToken submit_render_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens = {}) override
		{
			return submit(render_queue, cmds, gpu_wait_tokens);
		}

		SyncToken submit_compute_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens = {}) override
		{
			return submit(compute_queue, cmds, gpu_wait_tokens);
		}

		SyncToken submit_copy_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens = {}) override
		{
			return submit(copy_queue, cmds, gpu_wait_tokens);
		}

		SyncToken submit_for_present(ArrayView<CommandListHandle> cmds,
									 SwapChain&					  swap_chain,
									 ConstSpan<SyncToken>		  gpu_wait_tokens = {}) override
		{
			auto submit_token = submit(render_queue, cmds, gpu_wait_tokens);
			swap_chain.vulkan.present(submit_token.vulkan.semaphore, render_queue);
			return submit_token;
		}

		void wait_for_workload(SyncToken cpu_wait_token) override
		{
			uint64_t resets = cpu_wait_token.vulkan.resets;
			if(resets == 0 || resets != sync_tokens.get_current_resets(cpu_wait_token))
				return; // The token is either already reused, meaning its workload must be done, or default-initialized.

			auto result = api->vkWaitForFences(device.get(), 1, &cpu_wait_token.vulkan.fence, false, UINT64_MAX);
			VT_CHECK_RESULT(result, "Failed to wait for Vulkan fence.");
		}

		void flush_render_queue() override
		{
			auto result = api->vkQueueWaitIdle(render_queue);
			VT_CHECK_RESULT(result, "Failed to flush Vulkan render queue.");
		}

		void flush_compute_queue() override
		{
			auto result = api->vkQueueWaitIdle(compute_queue);
			VT_CHECK_RESULT(result, "Failed to flush Vulkan compute queue.");
		}

		void flush_copy_queue() override
		{
			auto result = api->vkQueueWaitIdle(copy_queue);
			VT_CHECK_RESULT(result, "Failed to flush Vulkan copy queue.");
		}

		void flush() override
		{
			auto result = api->vkDeviceWaitIdle(device.get());
			VT_CHECK_RESULT(result, "Failed to flush Vulkan device.");

			sync_tokens.await_all_fences(*api);
		}

	private:
		static constexpr char const* REQUIRED_DEVICE_EXTENSIONS[] = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		static constexpr VkPhysicalDeviceFeatures REQUIRED_FEATURES {};

		struct QueueFamilies
		{
			uint32_t render	 = UINT32_MAX;
			uint32_t compute = UINT32_MAX;
			uint32_t copy	 = UINT32_MAX;
		};

		struct DeviceDeleter
		{
			using pointer = VkDevice;
			void operator()(VkDevice device) const
			{
				InstanceApiTable::get().vkDestroyDevice(device, nullptr);
			}
		};
		using UniqueVkDevice = std::unique_ptr<VkDevice, DeviceDeleter>;

		struct AllocatorDeleter
		{
			using pointer = VmaAllocator;
			void operator()(VmaAllocator allocator) const
			{
				vmaDestroyAllocator(allocator);
			}
		};
		using UniqueVmaAllocator = std::unique_ptr<VmaAllocator, AllocatorDeleter>;

		VkPhysicalDevice				adapter;
		QueueFamilies					queue_families;
		UniqueVkDevice					device;
		std::unique_ptr<DeviceApiTable> api;
		SyncTokenPool					sync_tokens;
		DescriptorPool					descriptor_pool;
		UniqueVkPipelineCache			pipeline_cache;
		VkQueue							render_queue;
		VkQueue							compute_queue;
		VkQueue							copy_queue;
		UniqueVmaAllocator				allocator;

		static bool check_queue_flags(VkQueueFlags flags, VkQueueFlags wanted, VkQueueFlags unwanted)
		{
			return flags & wanted && !(flags & unwanted);
		}

		RenderTarget make_platform_render_target(RenderTargetSpecification const& spec) override
		{
			return {VulkanRenderTarget(spec, *api), {spec.width, spec.height}};
		}

		RenderTarget make_platform_render_target(SharedRenderTargetSpecification const& spec,
												 SwapChain const&						swap_chain,
												 unsigned								back_buffer_index) override
		{
			return {VulkanRenderTarget(spec, swap_chain, back_buffer_index, *api), swap_chain->get_size()};
		}

		void recreate_platform_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec) override
		{
			// We can just move-assign here because Vulkan needs no special code for render target recreation.
			render_target = make_platform_render_target(spec);
		}

		void recreate_platform_render_target(RenderTarget&							render_target,
											 SharedRenderTargetSpecification const& spec,
											 SwapChain const&						swap_chain,
											 unsigned								back_buffer_index) override
		{
			// We can just move-assign here because Vulkan needs no special code for render target recreation.
			render_target = make_platform_render_target(spec, swap_chain, back_buffer_index);
		}

		QueueFamilies query_queue_families() const
		{
			auto& driver = InstanceApiTable::get();

			uint32_t family_count;
			driver.vkGetPhysicalDeviceQueueFamilyProperties(adapter, &family_count, nullptr);
			SmallList<VkQueueFamilyProperties> queue_family_properties(family_count);
			driver.vkGetPhysicalDeviceQueueFamilyProperties(adapter, &family_count, queue_family_properties.data());

			QueueFamilies families;

			uint32_t index = 0;
			for(auto const& family : queue_family_properties)
			{
				auto flags = family.queueFlags;
				if(check_queue_flags(flags, VK_QUEUE_GRAPHICS_BIT, 0))
					families.render = index;

				if(check_queue_flags(flags, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT))
					families.compute = index;

				if(check_queue_flags(flags, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
					families.copy = index;

				++index;
			}
			VT_ENSURE(families.render != UINT32_MAX, "Vulkan device has no dedicated render queue.");
			VT_ENSURE(families.compute != UINT32_MAX, "Vulkan device has no dedicated compute queue.");
			VT_ENSURE(families.copy != UINT32_MAX, "Vulkan device has no dedicated copy queue.");

			return families;
		}

		UniqueVkDevice make_device() const
		{
			ensure_device_extensions_exist();
			ensure_features_exist();

			float const priorities[] {1.0f};

			VkDeviceQueueCreateInfo const queue_info {
				.sType			  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueCount		  = 1,
				.pQueuePriorities = priorities,
			};
			VkDeviceQueueCreateInfo queue_infos[] {queue_info, queue_info, queue_info};
			queue_infos[0].queueFamilyIndex = queue_families.render;
			queue_infos[1].queueFamilyIndex = queue_families.compute;
			queue_infos[2].queueFamilyIndex = queue_families.copy;

			VkDeviceCreateInfo const device_info {
				.sType					 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				.queueCreateInfoCount	 = count(queue_infos),
				.pQueueCreateInfos		 = queue_infos,
				.enabledLayerCount		 = 0,
				.ppEnabledLayerNames	 = nullptr,
				.enabledExtensionCount	 = count(REQUIRED_DEVICE_EXTENSIONS),
				.ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS,
				.pEnabledFeatures		 = &REQUIRED_FEATURES,
			};
			UniqueVkDevice fresh_device;

			auto result = InstanceApiTable::get().vkCreateDevice(adapter, &device_info, nullptr, std::out_ptr(fresh_device));
			VT_CHECK_RESULT(result, "Failed to create Vulkan device.");

			return fresh_device;
		}

		void ensure_device_extensions_exist() const
		{
			auto& driver = InstanceApiTable::get();

			uint32_t extension_count;
			auto	 result = driver.vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extension_count, nullptr);
			VT_CHECK_RESULT(result, "Failed to query Vulkan device extension count.");

			Array<VkExtensionProperties> extensions(extension_count);
			result = driver.vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extension_count, extensions.data());
			VT_CHECK_RESULT(result, "Failed to enumerate Vulkan device extensions.");

			for(std::string_view required_ext : REQUIRED_DEVICE_EXTENSIONS)
			{
				bool found = false;
				for(auto& extension : extensions)
				{
					if(extension.extensionName == required_ext)
					{
						found = true;
						break;
					}
				}
				VT_ENSURE(found, "Failed to find required Vulkan device extension.");
			}
		}

		void ensure_features_exist() const
		{
			using FeatureArray = std::array<VkBool32, sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32)>;

			VkPhysicalDeviceFeatures features;
			InstanceApiTable::get().vkGetPhysicalDeviceFeatures(adapter, &features);
			auto available_features = std::bit_cast<FeatureArray>(features);
			auto required_features	= std::bit_cast<FeatureArray>(REQUIRED_FEATURES);

			auto available = std::begin(available_features);
			for(auto required_feature : required_features)
			{
				if(required_feature)
					VT_ENSURE(*available, "Required Vulkan device feature is not available.");

				++available;
			}
		}

		void initialize_allocator()
		{
			auto& driver = InstanceApiTable::get();

			VmaVulkanFunctions const functions {
				.vkGetPhysicalDeviceProperties		 = driver.vkGetPhysicalDeviceProperties,
				.vkGetPhysicalDeviceMemoryProperties = driver.vkGetPhysicalDeviceMemoryProperties,
				.vkAllocateMemory					 = api->vkAllocateMemory,
				.vkFreeMemory						 = api->vkFreeMemory,
				.vkMapMemory						 = api->vkMapMemory,
				.vkUnmapMemory						 = api->vkUnmapMemory,
				.vkFlushMappedMemoryRanges			 = api->vkFlushMappedMemoryRanges,
				.vkInvalidateMappedMemoryRanges		 = api->vkInvalidateMappedMemoryRanges,
				.vkBindBufferMemory					 = api->vkBindBufferMemory,
				.vkBindImageMemory					 = api->vkBindImageMemory,
				.vkGetBufferMemoryRequirements		 = api->vkGetBufferMemoryRequirements,
				.vkGetImageMemoryRequirements		 = api->vkGetImageMemoryRequirements,
				.vkCreateBuffer						 = api->vkCreateBuffer,
				.vkDestroyBuffer					 = api->vkDestroyBuffer,
				.vkCreateImage						 = api->vkCreateImage,
				.vkDestroyImage						 = api->vkDestroyImage,
				.vkCmdCopyBuffer					 = api->vkCmdCopyBuffer,
			};
			VmaAllocatorCreateInfo const allocator_info {
				.flags							= VMA_ALLOCATOR_CREATE_EXTERNALLY_SYNCHRONIZED_BIT,
				.physicalDevice					= adapter,
				.device							= device.get(),
				.preferredLargeHeapBlockSize	= 0, // Uses the default block size.
				.pAllocationCallbacks			= nullptr,
				.pDeviceMemoryCallbacks			= nullptr,
				.frameInUseCount				= 0, // TODO: This probably needs to be changed.
				.pHeapSizeLimit					= nullptr,
				.pVulkanFunctions				= &functions,
				.pRecordSettings				= nullptr,
				.instance						= driver.instance,
				.vulkanApiVersion				= VK_API_VERSION_1_0,
				.pTypeExternalMemoryHandleTypes = nullptr,
			};
			auto result = vmaCreateAllocator(&allocator_info, std::out_ptr(allocator));
			VT_CHECK_RESULT(result, "Failed to create Vulkan memory allocator.");

			api->allocator = allocator.get();
		}

		void* map_resource(auto const& resource) const
		{
			void* ptr;
			auto  result = vmaMapMemory(allocator.get(), resource.vulkan.get_allocation(), &ptr);
			VT_CHECK_RESULT(result, "Failed to map Vulkan resource.");
			return ptr;
		}

		SyncToken submit(VkQueue queue, ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens)
		{
			SmallList<VkSemaphore> wait_semaphores(gpu_wait_tokens.size());

			auto semaphore = wait_semaphores.begin();
			for(auto& token : gpu_wait_tokens)
				*semaphore++ = token.vulkan.semaphore;

			auto token = sync_tokens.acquire_token(*api);

			VkPipelineStageFlags stages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

			VkSubmitInfo const submit {
				.sType				  = VK_STRUCTURE_TYPE_SUBMIT_INFO,
				.waitSemaphoreCount	  = count(wait_semaphores),
				.pWaitSemaphores	  = wait_semaphores.data(),
				.pWaitDstStageMask	  = &stages,
				.commandBufferCount	  = count(cmds),
				.pCommandBuffers	  = reinterpret_cast<VkCommandBuffer const*>(cmds.data()),
				.signalSemaphoreCount = 1,
				.pSignalSemaphores	  = &token.vulkan.semaphore,
			};
			auto result = api->vkQueueSubmit(queue, 1, &submit, token.vulkan.fence);
			VT_CHECK_RESULT(result, "Failed to submit to Vulkan queue.");

			return token;
		}
	};
}
