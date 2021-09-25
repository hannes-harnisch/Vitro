module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.RenderPass;

import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanRenderPass
	{
	public:
		VkRenderPass ptr() const
		{
			return render_pass.get();
		}

	private:
		UniqueVkRenderPass render_pass;
	};
}
