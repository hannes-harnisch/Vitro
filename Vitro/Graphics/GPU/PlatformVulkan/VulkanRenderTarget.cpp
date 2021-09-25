module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.RenderTarget;

import vt.Core.Rectangle;
import vt.Graphics.Vulkan.Driver;

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
