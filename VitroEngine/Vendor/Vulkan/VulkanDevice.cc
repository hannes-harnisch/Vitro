module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"

#include <memory>
#include <string_view>
#include <vector>
export module vt.Vulkan.Device;

import vt.Graphics.DeviceBase;
import vt.Graphics.Handle;
import vt.Vulkan.Driver;

namespace vt::vulkan
{
	class Queue
	{
	public:
	private:
	};

	export class VulkanDevice final : public DeviceBase
	{
	public:
		VulkanDevice(Adapter in_adapter) : adapter(in_adapter.vulkan)
		{
			ensure_device_extensions_exist();

			auto api = VulkanDriver::get_api();
		}

		SyncValue submit_render_commands(CSpan<CommandListHandle> command_lists, SyncValue gpu_sync) override
		{}

		SyncValue submit_compute_commands(CSpan<CommandListHandle> command_lists, SyncValue gpu_sync) override
		{}

		SyncValue submit_copy_commands(CSpan<CommandListHandle> command_lists, SyncValue gpu_sync) override
		{}

		void wait_for_workload(SyncValue cpu_sync) override
		{}

		void flush_render_queue() override
		{}

		void flush_compute_queue() override
		{}

		void flush_copy_queue() override
		{}

		void wait_for_idle() override
		{}

		DeviceFunctionTable const* get_api() const
		{
			return device_functions.get();
		}

	private:
		static constexpr char const* RequiredDeviceExtensions[] {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};

		std::unique_ptr<DeviceFunctionTable> device_functions;
		VkPhysicalDevice					 adapter;

		void ensure_device_extensions_exist()
		{
			auto api = VulkanDriver::get_api();

			unsigned count;
			auto	 result = api->vkEnumerateDeviceExtensionProperties(adapter, nullptr, &count, nullptr);
			VT_ENSURE_RESULT(result, "Failed to query Vulkan device extension count.");

			std::vector<VkExtensionProperties> extensions(count);
			result = api->vkEnumerateDeviceExtensionProperties(adapter, nullptr, &count, extensions.data());
			VT_ENSURE_RESULT(result, "Failed to enumerate Vulkan device extensions.");

			for(auto required_ext : RequiredDeviceExtensions)
			{
				bool found = false;
				for(auto& extension : extensions)
				{
					if(extension.extensionName == std::string_view(required_ext))
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
