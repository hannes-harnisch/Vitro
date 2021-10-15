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
import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanDevice final : public DeviceBase
	{
	public:
		VulkanDevice(Adapter in_adapter) : adapter(in_adapter.vulkan)
		{
			ensure_device_extensions_exist();
			auto instance_api = VulkanDriver::get_api();

			unsigned family_count;
			instance_api->vkGetPhysicalDeviceQueueFamilyProperties(adapter, &family_count, nullptr);
			std::vector<VkQueueFamilyProperties> queue_families(family_count);
			instance_api->vkGetPhysicalDeviceQueueFamilyProperties(adapter, &family_count, queue_families.data());

			unsigned index = 0;
			for(auto const& family : queue_families)
			{
				auto flags = family.queueFlags;
				if(check_queue_flags(flags, VK_QUEUE_GRAPHICS_BIT, 0))
					render_queue_family = index;

				if(check_queue_flags(flags, VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT))
					compute_queue_family = index;

				if(check_queue_flags(flags, VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT))
					copy_queue_family = index;

				++index;
			}
			VT_ENSURE(render_queue_family != ~0u, "Vulkan device has no dedicated render queue.");
			VT_ENSURE(compute_queue_family != ~0u, "Vulkan device has no dedicated compute queue.");
			VT_ENSURE(copy_queue_family != ~0u, "Vulkan device has no dedicated copy queue.");

			float const priorities[] {1.0f};

			VkDeviceQueueCreateInfo const queue_info {
				.sType			  = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueCount		  = 1,
				.pQueuePriorities = priorities,
			};
			std::array queue_infos {queue_info, queue_info, queue_info};
			queue_infos[0].queueFamilyIndex = render_queue_family;
			queue_infos[1].queueFamilyIndex = compute_queue_family;
			queue_infos[2].queueFamilyIndex = copy_queue_family;

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

			auto result = instance_api->vkCreateDevice(adapter, &device_info, nullptr, &unowned_device);
			device.reset(unowned_device);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan device.");
			api = std::make_unique<DeviceApiTable>(device.get());

			api->vkGetDeviceQueue(device.get(), render_queue_family, 0, &render_queue);
			api->vkGetDeviceQueue(device.get(), compute_queue_family, 0, &compute_queue);
			api->vkGetDeviceQueue(device.get(), copy_queue_family, 0, &copy_queue);

			initialize_queue_sync_objects(render_queue_fence, render_queue_semaphore);
			initialize_queue_sync_objects(compute_queue_fence, compute_queue_semaphore);
			initialize_queue_sync_objects(copy_queue_fence, copy_queue_semaphore);
		}

		CopyCommandList make_copy_command_list() override
		{
			return VulkanCommandList<CommandType::Copy>(copy_queue_family, *api);
		}

		ComputeCommandList make_compute_command_list() override
		{
			return VulkanCommandList<CommandType::Compute>(compute_queue_family, *api);
		}

		RenderCommandList make_render_command_list() override
		{
			return VulkanCommandList<CommandType::Render>(render_queue_family, *api);
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
				states.emplace_back(spec, *api);

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
		{}

		RenderTarget make_render_target(RenderTargetSpecification const& spec) override
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

		void recreate_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec) override
		{}

		SyncValue submit_render_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) override
		{}

		SyncValue submit_compute_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) override
		{}

		SyncValue submit_copy_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) override
		{}

		SyncValue submit_for_present(ArrayView<CommandListHandle> cmds,
									 SwapChain&					  swap_chain,
									 ConstSpan<SyncValue>		  gpu_syncs = {}) override
		{}

		void wait_for_workload(SyncValue cpu_sync) override
		{
			auto result = api->vkWaitForFences(device.get(), 1, &cpu_sync.vulkan.fence, true, UINT64_MAX);
			VT_ASSERT_RESULT(result, "Failed to wait for Vulkan fence.");

			result = api->vkResetFences(device.get(), 1, &cpu_sync.vulkan.fence);
			VT_ASSERT_RESULT(result, "Failed to reset Vulkan fence.");
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

		SwapChain make_swap_chain(Driver& driver, Window& window, uint8_t buffer_count = SwapChain::DEFAULT_BUFFERS) override
		{}

		RenderTarget make_render_target(SharedRenderTargetSpecification const& spec,
										SwapChain const&					   swap_chain,
										unsigned							   back_buffer_index) override
		{}

		void recreate_render_target(RenderTarget&						   render_target,
									SharedRenderTargetSpecification const& spec,
									SwapChain const&					   swap_chain,
									unsigned							   back_buffer_index) override
		{}

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

		std::unique_ptr<DeviceApiTable const> api;

		VkPhysicalDevice	   adapter;
		UniqueVkDevice		   device;
		unsigned			   render_queue_family	= ~0u;
		unsigned			   compute_queue_family = ~0u;
		unsigned			   copy_queue_family	= ~0u;
		VkQueue				   render_queue;
		UniqueVkFence		   render_queue_fence;
		UniqueVkSemaphore	   render_queue_semaphore;
		VkQueue				   compute_queue;
		UniqueVkFence		   compute_queue_fence;
		UniqueVkSemaphore	   compute_queue_semaphore;
		VkQueue				   copy_queue;
		UniqueVkFence		   copy_queue_fence;
		UniqueVkSemaphore	   copy_queue_semaphore;
		UniqueVkDescriptorPool descriptor_pool;
		UniqueVkPipelineCache  pipeline_cache;

		static bool check_queue_flags(VkQueueFlags flags, VkQueueFlags wanted, VkQueueFlags unwanted)
		{
			return flags & wanted && !(flags & unwanted);
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

		void initialize_queue_sync_objects(UniqueVkFence& fence, UniqueVkSemaphore& semaphore) const
		{
			VkFenceCreateInfo const fence_info {
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			};
			VkFence unowned_fence;

			auto result = api->vkCreateFence(device.get(), &fence_info, nullptr, &unowned_fence);
			fence.reset(unowned_fence, *api);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan fence for queue.");

			VkSemaphoreCreateInfo const semaphore_info {
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};
			VkSemaphore unowned_semaphore;
			result = api->vkCreateSemaphore(device.get(), &semaphore_info, nullptr, &unowned_semaphore);
			semaphore.reset(unowned_semaphore, *api);
			VT_ENSURE_RESULT(result, "Failed to create Vulkan semaphore for queue.");
		}
	};
}
