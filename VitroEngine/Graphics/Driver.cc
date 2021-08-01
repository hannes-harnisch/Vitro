export module Vitro.Graphics.Driver;

import Vitro.Core.Singleton;
import Vitro.Graphics.DriverBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.Driver;
#endif
import Vitro.VT_GPU_API_MODULE.Driver;

namespace vt
{
	export struct Driver :
		InterfaceVariant<DriverBase,
#if VT_DYNAMIC_GPU_API
						 VT_GPU_API_NAME_PRIMARY::Driver,
#endif
						 VT_GPU_API_NAME::Driver>,
		Singleton<Driver>
	{
		using InterfaceVariant<DriverBase,
#if VT_DYNAMIC_GPU_API
							   VT_GPU_API_NAME_PRIMARY::Driver,
#endif
							   VT_GPU_API_NAME::Driver>::InterfaceVariant;
	};
}
