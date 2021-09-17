module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"
export module vt.Vulkan.Pipeline;

import vt.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanPipeline
	{
	public:
		VulkanPipeline()
		{}

		VkPipeline ptr() const
		{
			pipeline.get();
		}

	private:
		UniqueVkPipeline pipeline;
	};
}
