module;
#include "Core/Macros.hpp"
#include "VulkanAPI.hpp"
export module vt.Graphics.Vulkan.Pipeline;

import vt.Graphics.PipelineSpecification;
import vt.Graphics.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanRenderPipeline
	{
	public:
		VulkanRenderPipeline(RenderPipelineSpecification const& spec)
		{}

		VkPipeline ptr() const
		{
			return pipeline.get();
		}

	private:
		UniqueVkPipeline pipeline;
	};

	export class VulkanComputePipeline
	{
	public:
		VulkanComputePipeline(ComputePipelineSpecification const& spec)
		{}

		VkPipeline ptr() const
		{
			return pipeline.get();
		}

	private:
		UniqueVkPipeline pipeline;
	};
}
