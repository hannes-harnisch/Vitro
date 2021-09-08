module;
#include "Core/Macros.hh"
export module vt.Graphics.Driver;

import vt.Graphics.DriverBase;
import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.Driver;
#endif
import vt.VT_GPU_API_MODULE.Driver;

namespace vt
{
	export using Driver			   = InterfaceVariant<DriverBase, VT_GPU_API_VARIANT_ARGS(Driver)>;
	export using CommandListHandle = ResourceVariant<VT_GPU_API_VARIANT_ARGS(CommandListHandle)>;
}
