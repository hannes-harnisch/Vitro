module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.RenderTarget;

import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanRenderTarget
	{
	public:
		VulkanRenderTarget(RenderTargetSpecification const& spec, DeviceApiTable const& api)
		{}

		VkFramebuffer get_framebuffer() const
		{
			return frame_buffer.get();
		}

	private:
		UniqueVkFramebuffer frame_buffer;
	};
}
