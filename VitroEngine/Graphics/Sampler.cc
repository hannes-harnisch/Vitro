module;
#include "Core/Macros.hh"
export module vt.Graphics.Sampler;

import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.Sampler;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.Sampler;
#endif

namespace vt
{
	export using Sampler = ResourceVariant<VT_GPU_API_VARIANT_ARGS(Sampler)>;
}
