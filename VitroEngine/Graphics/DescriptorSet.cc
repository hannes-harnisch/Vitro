module;
#include "Core/Macros.hh"
export module vt.Graphics.DescriptorSet;

import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.DescriptorSet;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.DescriptorSet;
#endif

namespace vt
{
	export using DescriptorSet = ResourceVariant<VT_GPU_API_VARIANT_ARGS(DescriptorSet)>;
}
