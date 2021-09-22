module;
#include "Core/Macros.hh"
export module vt.Graphics.Resource;

import vt.Graphics.DynamicGpuApi;

import vt.VT_GPU_API_MODULE.Buffer;
import vt.VT_GPU_API_MODULE.Pipeline;
import vt.VT_GPU_API_MODULE.Texture;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.Buffer;
import vt.VT_GPU_API_MODULE_SECONDARY.Pipeline;
import vt.VT_GPU_API_MODULE_SECONDARY.Texture;
#endif

namespace vt
{
	export using BufferResource = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Buffer)>;
	export class Buffer : public BufferResource
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

	export using ComputePipeline = ResourceVariant<VT_GPU_API_VARIANT_ARGS(ComputePipeline)>;
	export using RenderPipeline	 = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RenderPipeline)>;
	export using Texture		 = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Texture)>;
}
