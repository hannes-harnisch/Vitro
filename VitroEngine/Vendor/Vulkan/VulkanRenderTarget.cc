module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"
export module vt.Vulkan.RenderTarget;

import vt.Core.Rectangle;
import vt.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanRenderTarget
	{
	public:
		VkFramebuffer get_frame_buffer() const
		{
			return frame_buffer.get();
		}

		unsigned get_width() const
		{
			return extent.width;
		}

		unsigned get_height() const
		{
			return extent.height;
		}

	private:
		UniqueVkFramebuffer frame_buffer;
		Extent				extent;
	};
}
