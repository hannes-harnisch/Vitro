module;
#include "Core/Macros.hh"
export module vt.Graphics.RootSignature;

import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.RootSignature;
#endif
import vt.VT_GPU_API_MODULE.RootSignature;

namespace vt
{
	export using RootSignature = ResourceVariant<VT_GPU_API_VARIANT_ARGS(RootSignature)>;
}
