module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"
export module vt.Vulkan.RootSignature;

import vt.Vulkan.Driver;

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
