module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <array>
#include <memory>
#include <string_view>
#include <vector>
export module vt.Graphics.Vulkan.Device;

import vt.Core.Array;
import vt.Graphics.DeviceBase;
import vt.Graphics.Handle;
import vt.Graphics.RingBuffer;
import vt.Graphics.Vulkan.DescriptorPool;
import vt.Graphics.Vulkan.Driver;
import vt.Graphics.Vulkan.Handle;

namespace vt::vulkan
{
	struct UniqueSyncToken
	{
		UniqueVkFence	  fence;
		UniqueVkSemaphore semaphore;
		uint64_t		  resets = 0;

		UniqueSyncToken(DeviceApiTable const& api)
		{
			VkFenceCreateInfo const fence_info {
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT,
			};
			VkFence unowned_fence;

			auto result = api.vkCreateFence(api.device, &fence_info, nullptr, &unowned_fence);
			fence.reset(unowned_fence, api);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan fence for queue.");

			VkSemaphoreCreateInfo const semaphore_info {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};
			VkSemaphore unowned_semaphore;
			result = api.vkCreateSemaphore(api.device, &semaphore_info, nullptr, &unowned_semaphore);
			semaphore.reset(unowned_semaphore, api);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan semaphore for queue.");
		}

		SyncToken get_unowned() const
		{
			return {
				fence.get(),
				semaphore.get(),
				resets,
			};
		}
	};

	class SyncTokenPool
	{
	public:
		SyncTokenPool(DeviceApiTable const& api)
		{
			for(int i = 0; i != INITIAL_COUNT; ++i)
				tokens.emplace_back(api);
		}

		SyncToken acquire_token(DeviceApiTable const& api)
		{
			size_t size = tokens.size();
			size_t i	= current_index;
			do
			{
				auto& current_token = tokens[i];

				auto status = api.vkGetFenceStatus(api.device, current_token.fence.get());
				if(status == VK_SUCCESS)
				{
					auto acquired_token = current_token.get_unowned();

					auto result = api.vkResetFences(api.device, 1, &acquired_token.vulkan.fence);
					VT_ASSERT_RESULT(result, "Failed to reset Vulkan fence.");
					current_token.resets++;

					advance_index();
					return acquired_token;
				}

				if(i == size - 1)
					i = 0;
				else
					++i;
			} while(i != current_index);

			// All existing tokens are somehow still not ready, so grow the pool and return the new one.
			auto new_token = tokens.emplace(tokens.begin() + current_index, api);
			advance_index();
			return new_token->get_unowned();
		}

		uint64_t get_current_resets(SyncToken const& token) const
		{
			auto it = std::find_if(tokens.begin(), tokens.end(), [&](UniqueSyncToken const& unique_token) {
				return unique_token.fence.get() == token.vulkan.fence;
			});
			return it->resets;
		}

	private:
		static constexpr size_t INITIAL_COUNT = 30;

		std::vector<UniqueSyncToken> tokens;
		size_t						 current_index = 0;

		void advance_index()
		{
			current_index = (current_index + 1) % tokens.size();
		}
	};

	export class VulkanDevice final : public DeviceBase
	{
	public:
		VulkanDevice(Adapter in_adapter) :
			adapter(in_adapter.vulkan),
			queue_families(query_queue_families()),
			device(make_device()),
			api(std::make_unique<DeviceApiTable>(device.get())),
			sync_tokens(*api),
			descriptor_pool(*api)
		{
			api->vkGetDeviceQueue(device.get(), queue_families.render, 0, &render_queue);
			api->vkGetDeviceQueue(device.get(), queue_families.compute, 0, &compute_queue);
			api->vkGetDeviceQueue(device.get(), queue_families.copy, 0, &copy_queue);
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

		std::vector<ComputePipeline> make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs) override
		{
			std::vector<VkComputePipelineCreateInfo> pipeline_infos;
			pipeline_infos.reserve(specs.size());
			for(auto const& spec : specs)
				pipeline_infos.emplace_back(VkComputePipelineCreateInfo {
					.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
					.stage {
						.sType				 = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
						.stage				 = VK_SHADER_STAGE_COMPUTE_BIT,
						.module				 = spec.compute_shader.vulkan.ptr(),
						.pName				 = "main",
						.pSpecializationInfo = nullptr,
					},
					.layout				= spec.root_signature.vulkan.ptr(),
					.basePipelineHandle = nullptr,
					.basePipelineIndex	= 0,
				});

			Array<VkPipeline> pipelines(specs.size());

			auto result = api->vkCreateComputePipelines(device.get(), pipeline_cache.get(), count(pipeline_infos),
														pipeline_infos.data(), nullptr, pipelines.data());

			std::vector<ComputePipeline> compute_pipelines;
			compute_pipelines.reserve(specs.size());
			for(auto pipeline : pipelines)
				compute_pipelines.emplace_back(VulkanComputePipeline(pipeline, *api));

			VT_ENSURE_RESULT(result, "Failed to create Vulkan graphics pipelines.");
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
				pipeline_infos.emplace_back(VkGraphicsPipelineCreateInfo {
					.sType				 = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
					.stageCount			 = count(state.shader_stages),
					.pStages			 = state.shader_stages.data(),
					.pVertexInputState	 = &state.vertex_input,
					.pInputAssemblyState = &state.input_assembly,
					.pTessellationState	 = &state.tessellation,
					.pViewportState		 = &state.viewport,
					.pRasterizationState = &state.rasterization,
					.pMultisampleState	 = &state.multisample,
					.pDepthStencilState	 = &state.depth_stencil,
					.pColorBlendState	 = &state.color_blend,
					.pDynamicState		 = &state.dynamic_state,
					.layout				 = state.pipeline_layout,
					.renderPass			 = state.render_pass,
					.subpass			 = state.subpass_index,
					.basePipelineHandle	 = nullptr,
					.basePipelineIndex	 = 0,
				});

			Array<VkPipeline> pipelines(specs.size());

			auto result = api->vkCreateGraphicsPipelines(device.get(), pipeline_cache.get(), count(pipeline_infos),
														 pipeline_infos.data(), nullptr, pipelines.data());

			std::vector<RenderPipeline> render_pipelines;
			render_pipelines.reserve(specs.size());
			for(auto pipeline : pipelines)
				render_pipelines.emplace_back(VulkanRenderPipeline(pipeline, *api));

			VT_ENSURE_RESULT(result, "Failed to create Vulkan graphics pipelines.");
			return render_pipelines;
		}

		std::vector<DescriptorSet> make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts) override
		{
			return {};
		}

		DescriptorSetLayout make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec) override
		{
			return VulkanDescriptorSetLayout(spec, *api);
		}

		RenderPass make_render_pass(RenderPassSpecification const& spec) override
		{
			return VulkanRenderPass(spec, *api);
		}

		RenderTarget make_render_target(RenderTargetSpecification const& spec) override
		{
			return VulkanRenderTarget(spec, *api);
		}

		RenderTarget make_render_target(SharedRenderTargetSpecification const& spec,
										SwapChain const&					   swap_chain,
										unsigned							   back_buffer_index) override
		{}

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

		SwapChain make_swap_chain(Driver& driver, Window& window, uint8_t buffer_count = SwapChain::DEFAULT_BUFFERS) override
		{}

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
		{}

		void wait_for_workload(SyncToken cpu_wait_token) override
		{
			if(sync_tokens.get_current_resets(cpu_wait_token) > cpu_wait_token.vulkan.resets)
				return;

			auto result = api->vkWaitForFences(device.get(), 1, &cpu_wait_token.vulkan.fence, false, ~0ull);
			VT_ASSERT_RESULT(result, "Failed to wait for Vulkan fence.");
		}

		void flush_render_queue() override
		{
			api->vkQueueWaitIdle(render_queue);
		}

		void flush_compute_queue() override
		{
			api->vkQueueWaitIdle(compute_queue);
		}

		void flush_copy_queue() override
		{
			api->vkQueueWaitIdle(copy_queue);
		}

		void wait_for_idle() override
		{
			api->vkDeviceWaitIdle(device.get());
		}

	private:
		static constexpr std::array REQUIRED_DEVICE_EXTENSIONS = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		static constexpr VkPhysicalDeviceFeatures REQUIRED_FEATURES {};

		struct DeviceDeleter
		{
			using pointer = VkDevice;
			void operator()(VkDevice device) const
			{
				VulkanDriver::get_api()->vkDestroyDevice(device, nullptr);
			}
		};
		using UniqueVkDevice = std::unique_ptr<VkDevice, DeviceDeleter>;

		VkPhysicalDevice adapter;
		struct QueueFamilies
		{
			unsigned render	 = ~0u;
			unsigned compute = ~0u;
			unsigned copy	 = ~0u;
		} queue_families;
		UniqueVkDevice						  device;
		std::unique_ptr<DeviceApiTable const> api;
		SyncTokenPool						  sync_tokens;
		DescriptorPool						  descriptor_pool;
		UniqueVkPipelineCache				  pipeline_cache;
		VkQueue								  render_queue;
		VkQueue								  compute_queue;
		VkQueue								  copy_queue;

		static bool check_queue_flags(VkQueueFlags flags, VkQueueFlags wanted, VkQueueFlags unwanted)
		{
			return flags & wanted && !(flags & unwanted);
		}

		void recreate_platform_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec) override
		{}

		void recreate_platform_render_target(RenderTarget&							render_target,
											 SharedRenderTargetSpecification const& spec,
											 SwapChain const&						swap_chain,
											 unsigned								back_buffer_index) override
		{}

		QueueFamilies query_queue_families() const
		{
			auto instance_api = VulkanDriver::get_api();

			unsigned family_count;
			instance_api->vkGetPhysicalDeviceQueueFamilyProperties(adapter, &family_count, nullptr);
			std::vector<VkQueueFamilyProperties> queue_family_properties(family_count);
			instance_api->vkGetPhysicalDeviceQueueFamilyProperties(adapter, &family_count, queue_family_properties.data());

			QueueFamilies families;

			unsigned index = 0;
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
			VT_ENSURE(families.render != ~0u, "Vulkan device has no dedicated render queue.");
			VT_ENSURE(families.compute != ~0u, "Vulkan device has no dedicated compute queue.");
			VT_ENSURE(families.copy != ~0u, "Vulkan device has no dedicated copy queue.");

			return families;
		}

		UniqueVkDevice make_device() const
		{
			ensure_device_extensions_exist();

			float const priorities[] {1.0f};

			VkDeviceQueueCreateInfo const queue_info {
				.sType			  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueCount		  = 1,
				.pQueuePriorities = priorities,
			};
			std::array queue_infos {queue_info, queue_info, queue_info};
			queue_infos[0].queueFamilyIndex = queue_families.render;
			queue_infos[1].queueFamilyIndex = queue_families.compute;
			queue_infos[2].queueFamilyIndex = queue_families.copy;

			VkDeviceCreateInfo const device_info {
				.sType					 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
				.queueCreateInfoCount	 = count(queue_infos),
				.pQueueCreateInfos		 = queue_infos.data(),
				.enabledLayerCount		 = count(VulkanDriver::REQUIRED_LAYERS),
				.ppEnabledLayerNames	 = VulkanDriver::REQUIRED_LAYERS.data(),
				.enabledExtensionCount	 = count(REQUIRED_DEVICE_EXTENSIONS),
				.ppEnabledExtensionNames = REQUIRED_DEVICE_EXTENSIONS.data(),
				.pEnabledFeatures		 = &REQUIRED_FEATURES,
			};
			VkDevice unowned_device;

			auto instance_api = VulkanDriver::get_api();
			auto result		  = instance_api->vkCreateDevice(adapter, &device_info, nullptr, &unowned_device);

			UniqueVkDevice fresh_device(unowned_device);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan device.");
			return fresh_device;
		}

		void ensure_device_extensions_exist() const
		{
			auto instance_api = VulkanDriver::get_api();

			unsigned extension_count;
			auto	 result = instance_api->vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extension_count, nullptr);
			VT_ENSURE_RESULT(result, "Failed to query Vulkan device extension count.");

			std::vector<VkExtensionProperties> extensions(extension_count);
			result = instance_api->vkEnumerateDeviceExtensionProperties(adapter, nullptr, &extension_count, extensions.data());
			VT_ENSURE_RESULT(result, "Failed to enumerate Vulkan device extensions.");

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
			VulkanDriver::get_api()->vkGetPhysicalDeviceFeatures(adapter, &features);
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

		SyncToken submit(VkQueue queue, ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens)
		{
			std::vector<VkSemaphore> wait_semaphores;
			wait_semaphores.reserve(gpu_wait_tokens.size());
			for(auto& token : gpu_wait_tokens)
				wait_semaphores.emplace_back(token.vulkan.semaphore);

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
			VT_ENSURE_RESULT(result, "Failed to submit to Vulkan queue.");
			return token;
		}
	};
}
