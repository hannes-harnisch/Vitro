module;
#include "Core/Macros.hpp"
export module vt.Graphics.AssetResource;

import vt.Graphics.Device;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.VT_GPU_API_MODULE.Buffer;
import vt.Graphics.VT_GPU_API_MODULE.Pipeline;
import vt.Graphics.VT_GPU_API_MODULE.Texture;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Buffer;
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Pipeline;
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.Texture;
#endif

namespace vt
{
	using PlatformBuffer = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Buffer)>;
	export class Buffer : public PlatformBuffer
	{
	public:
		Buffer(Device&)
		{}

		unsigned get_size() const
		{
			return size;
		}

		unsigned get_stride() const
		{
			return stride;
		}

	private:
		unsigned size	= 0;
		unsigned stride = 0;
	};

	using PlatformPipeline = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Pipeline)>;

	export class ComputePipeline : public PlatformPipeline
	{
	public:
		ComputePipeline(Device& device, ComputePipelineSpecification const& spec) : PlatformPipeline(device, spec)
		{}
	};
	export class RenderPipeline : public PlatformPipeline
	{
	public:
		RenderPipeline(Device& device, RenderPipelineSpecification const& spec) : PlatformPipeline(device, spec)
		{}
	};

	using PlatformTexture = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Texture)>;
	export class Texture : public PlatformTexture
	{
	public:
		Texture(Device&)
		{}
	};
}
