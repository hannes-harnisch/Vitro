module;
#include "Core/Macros.hpp"

#include <array>
#include <memory>
#include <string_view>
#include <vector>
#include <vulkan/vulkan.h>
export module vt.Graphics.Vulkan.Device;

import vt.Graphics.DeviceBase;
import vt.Graphics.Handle;
import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	class Queue
	{
	public:
		VkQueue ptr()
		{
			return queue;
		}

	private:
		VkQueue queue;
	};

	export class VulkanDevice final : public DeviceBase
	{
	public:
		VulkanDevice(Adapter in_adapter) : adapter(in_adapter.vulkan)
		{
			ensure_device_extensions_exist();

			auto	 instance_api = VulkanDriver::get_api();
			unsigned count;
			auto	 result = instance_api->vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count, nullptr);
			VT_ENSURE_RESULT(result, "Failed to query Vulkan queue family count.");

			std::vector<VkQueueFamilyProperties> properties(count);
			result = instance_api->vkGetPhysicalDeviceQueueFamilyProperties(adapter, &count, properties.data());
			VT_ENSURE_RESULT(result, "Failed to enumerate Vulkan queue family properties.");

			for(auto& queue_family : properties)
			{}
		}

		std::vector<ComputePipeline> make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs) override
		{}

		std::vector<RenderPipeline> make_render_pipelines(ArrayView<RenderPipelineSpecification> specs) override
		{}

		std::vector<DescriptorSet> make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts) override
		{}

		DescriptorSetLayout make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec) override
		{}

		RenderPass make_render_pass(RenderPassSpecification const& spec) override
		{}

		RenderTarget make_render_target(RenderTargetSpecification const& spec) override
		{}

		RootSignature make_root_signature(RootSignatureSpecification const& spec) override
		{}

		Sampler make_sampler(SamplerSpecification const& spec) override
		{}

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
			vkapi->vkQueueWaitIdle(render_queue.ptr());
		}

		void flush_compute_queue() override
		{
			vkapi->vkQueueWaitIdle(compute_queue.ptr());
		}

		void flush_copy_queue() override
		{
			vkapi->vkQueueWaitIdle(copy_queue.ptr());
		}

		void wait_for_idle() override
		{
			vkapi->vkDeviceWaitIdle(device.get());
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

		DeviceFunctionTable const* api() const
		{
			return vkapi.get();
		}

	private:
		static constexpr std::array REQUIRED_DEVICE_EXTENSIONS = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		std::unique_ptr<DeviceFunctionTable> vkapi;
		VkPhysicalDevice					 adapter;

		struct DeviceDeleter
		{
			using pointer = VkDevice;
			void operator()(VkDevice device)
			{
				VulkanDriver::api()->vkDestroyDevice(device);
			}
		};
		std::unqiue_ptr<VkDevice, DeviceDeleter> device;

		Queue				   render_queue;
		Queue				   compute_queue;
		Queue				   copy_queue;
		UniqueVkDescriptorPool descriptor_pool;
		UniqueVkPipelineCache  pipeline_cache;

		void ensure_device_extensions_exist()
		{
			auto api = VulkanDriver::get_api();

			unsigned count;
			auto	 result = api->vkEnumerateDeviceExtensionProperties(adapter, nullptr, &count, nullptr);
			VT_ENSURE_RESULT(result, "Failed to query Vulkan device extension count.");

			std::vector<VkExtensionProperties> extensions(count);
			result = api->vkEnumerateDeviceExtensionProperties(adapter, nullptr, &count, extensions.data());
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
	};
}
