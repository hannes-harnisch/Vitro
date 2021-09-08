module;
#include "Core/Macros.hh"
export module vt.Graphics.DescriptorPool;

import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.DescriptorPool;
#endif
import vt.VT_GPU_API_MODULE.DescriptorPool;

namespace vt
{
	export using DescriptorPool = ResourceVariant<VT_GPU_API_VARIANT_ARGS(DescriptorPool)>;
}
