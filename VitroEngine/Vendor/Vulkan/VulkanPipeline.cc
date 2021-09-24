module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"
export module vt.Vulkan.Pipeline;

import vt.Graphics.Device;
import vt.Graphics.PipelineSpecification;
import vt.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanRenderPipeline
	{
	public:
		VulkanRenderPipeline(Device const& device, RenderPipelineSpecification const& spec)
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
		VulkanComputePipeline(Device const& device, ComputePipelineSpecification const& spec)
		{}

		VkPipeline ptr() const
		{
			return pipeline.get();
		}

	private:
		UniqueVkPipeline pipeline;
	};
}
