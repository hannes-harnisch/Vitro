module;
#include "Core/Macros.hh"
export module vt.Graphics.DescriptorSet;

import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.DescriptorSet;
#endif
import vt.VT_GPU_API_MODULE.DescriptorSet;

namespace vt
{
	export using DescriptorSet = ResourceVariant<VT_GPU_API_VARIANT_ARGS(DescriptorSet)>;
}
