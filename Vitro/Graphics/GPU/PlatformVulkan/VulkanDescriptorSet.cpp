module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.DescriptorSet;

namespace vt::vulkan
{
	export class VulkanDescriptorSet
	{
	public:
		VulkanDescriptorSet(VkDescriptorSet set, VkDescriptorSetLayout layout) : set(set), layout(layout)
		{}

		VkDescriptorSet get_handle() const
		{
			return set;
		}

		VkDescriptorSetLayout get_layout() const
		{
			return layout;
		}

	private:
		VkDescriptorSet		  set;
		VkDescriptorSetLayout layout;
	};
}
