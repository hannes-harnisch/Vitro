module;
#include "Core/Macros.hh"
export module vt.Graphics.Driver;

import vt.Graphics.DriverBase;
import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.Driver;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.Driver;
#endif

namespace vt
{
	export using Driver = InterfaceVariant<DriverBase, VT_GPU_API_VARIANT_ARGS(Driver)>;
}
