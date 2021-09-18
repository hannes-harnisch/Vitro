module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"
export module vt.Vulkan.RenderPass;

import vt.Vulkan.Driver;

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
