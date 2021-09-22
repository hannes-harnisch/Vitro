module;
#include "Core/Macros.hh"
#include "VulkanAPI.hh"
export module vt.Vulkan.Pipeline;

import vt.Graphics.Device;
import vt.Graphics.PipelineInfo;
import vt.Vulkan.Driver;

namespace vt::vulkan
{
	export class VulkanRenderPipeline
	{
	public:
		VulkanRenderPipeline(Device const& device, RenderPipelineInfo const& info)
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
		VulkanComputePipeline(Device const& device, ComputePipelineInfo const& info)
		{}

		VkPipeline ptr() const
		{
			return pipeline.get();
		}

	private:
		UniqueVkPipeline pipeline;
	};
}
