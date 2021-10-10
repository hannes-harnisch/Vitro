module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.DescriptorSet;

namespace vt::vulkan
{
	export class VulkanDescriptorSet
	{
	public:
		VkDescriptorSet ptr() const
		{
			return descriptor_set;
		}

	private:
		VkDescriptorSet descriptor_set;
	};
}
