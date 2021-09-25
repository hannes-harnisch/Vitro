module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.RootSignature;

import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanRootSignature
	{
	public:
		VkPipelineLayout ptr() const
		{
			return layout.get();
		}

	private:
		UniqueVkPipelineLayout layout;
	};
}
