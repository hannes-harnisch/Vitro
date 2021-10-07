module;
#include "Core/Macros.hpp"
export module vt.Graphics.AssetResource;

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

	using PlatformComputePipeline = ResourceVariant<VT_GPU_API_VARIANT_ARGS(ComputePipeline)>;
	export class ComputePipeline : public PlatformComputePipeline
	{
		using PlatformComputePipeline::PlatformComputePipeline;
	};

	using PlatformRenderPipeline = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderPipeline)>;
	export class RenderPipeline : public PlatformRenderPipeline
	{
		using PlatformRenderPipeline::PlatformRenderPipeline;
	};

	using PlatformTexture = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Texture)>;
	export class Texture : public PlatformTexture
	{};
}
