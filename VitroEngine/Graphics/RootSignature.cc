module;
#include "Core/Macros.hh"
export module vt.Graphics.RootSignature;

import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.RootSignature;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.RootSignature;
#endif

namespace vt
{
	export using RootSignature = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RootSignature)>;
}
