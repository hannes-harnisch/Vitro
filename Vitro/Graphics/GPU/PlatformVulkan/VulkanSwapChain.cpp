module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.SwapChain;

import vt.Core.Rectangle;
import vt.Graphics.SwapChainBase;

namespace vt::vulkan
{
	export class VulkanSwapChain final : public SwapChainBase
	{
	public:
		VulkanSwapChain()
		{
			VkFenceCreateInfo const fence_info {
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT,
			};
		}

		unsigned get_current_image_index() const override
		{
			return 0;
		}

		unsigned get_buffer_count() const override
		{
			return 0;
		}

		unsigned get_width() const override
		{
			return 0;
		}

		unsigned get_height() const override
		{
			return 0;
		}

		void resize(Extent) override
		{}

		void enable_vsync() override
		{}

		void disable_vsync() override
		{}

	private:
	};
}
