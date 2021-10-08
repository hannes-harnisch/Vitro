module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.RenderTarget;

import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanRenderTarget
	{
	public:
		VkFramebuffer get_framebuffer() const
		{
			return frame_buffer.get();
		}

	private:
		UniqueVkFramebuffer frame_buffer;
	};
}
