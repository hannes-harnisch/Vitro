module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"

#include <array>
#include <memory>
#include <string_view>
#include <vector>
export module vt.Graphics.Vulkan.Device;

import vt.Core.Algorithm;
import vt.Graphics.DeviceBase;
import vt.Graphics.Handle;
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
		}

		CopyCommandList make_copy_command_list() override
		{
			return VulkanCommandList<CommandType::Copy>(*api, copy_queue_family);
		}

		ComputeCommandList make_compute_command_list() override
		{
			return VulkanCommandList<CommandType::Compute>(*api, compute_queue_family);
		}

		RenderCommandList make_render_command_list() override
		{
			return VulkanCommandList<CommandType::Render>(*api, render_queue_family);
		}

		std::vector<ComputePipeline> make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs) override
		{
			return {};
		}

		std::vector<RenderPipeline> make_render_pipelines(ArrayView<RenderPipelineSpecification> specs) override
		{
			return {};
		}

		std::vector<DescriptorSet> make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts) override
		{
			return {};
		}

		DescriptorSetLayout make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec) override
		{}

		RenderPass make_render_pass(RenderPassSpecification const& spec) override
		{}

		RenderTarget make_render_target(RenderTargetSpecification const& spec) override
		{}

		RootSignature make_root_signature(RootSignatureSpecification const& spec) override
		{}

		Sampler make_sampler(SamplerSpecification const& spec) override
		{
			return VulkanSampler(*api, spec);
		}

		void recreate_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec) override
		{}

		SyncValue submit_render_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) override
		{}

		SyncValue submit_compute_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) override
		{}

		SyncValue submit_copy_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) override
		{}

		void wait_for_workload(SyncValue cpu_sync) override
		{}

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
			void operator()(VkDevice device)
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
		VkQueue				   compute_queue;
		VkQueue				   copy_queue;
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
	};
}
