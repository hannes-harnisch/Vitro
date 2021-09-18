module;
#include "Core/Macros.hh"
export module vt.Graphics.DescriptorPool;

import vt.Graphics.DescriptorPoolBase;
import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.DescriptorPool;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.DescriptorPool;
#endif

namespace vt
{
	export using DescriptorPool = InterfaceVariant<DescriptorPoolBase, VT_GPU_API_VARIANT_ARGS(DescriptorPool)>;
}
