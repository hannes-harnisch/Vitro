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
	export using Buffer	  = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Buffer)>;
	export using Pipeline = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Pipeline)>;
	export using Texture  = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Texture)>;
}
