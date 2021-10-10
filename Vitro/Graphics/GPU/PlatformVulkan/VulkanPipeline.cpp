module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.Pipeline;

import vt.Graphics.PipelineSpecification;
import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	class Pipeline
	{
	public:
		VkPipeline ptr() const
		{
			return pipeline.get();
		}

	protected:
		UniqueVkPipeline pipeline;
	};

	export class VulkanRenderPipeline : public Pipeline
	{
	public:
		VulkanRenderPipeline()
		{}
	};

}
