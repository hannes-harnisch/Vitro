export module Vitro.Graphics.Driver;

import Vitro.Graphics.DriverBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.Driver;
#endif
import Vitro.VT_GPU_API_MODULE.Driver;

namespace vt
{
	export using Driver = InterfaceVariant<DriverBase,
#if VT_DYNAMIC_GPU_API
										   VT_GPU_API_NAME_PRIMARY::Driver,
#endif
										   VT_GPU_API_NAME::Driver>;

#if VT_DYNAMIC_GPU_API
	export union CommandListHandle
	{
		VT_GPU_API_NAME_PRIMARY::CommandListHandle VT_GPU_API_NAME_PRIMARY;
		VT_GPU_API_NAME::CommandListHandle		   VT_GPU_API_NAME;
	};
#else
	export struct CommandListHandle
	{
		VT_GPU_API_NAME::CommandListHandle VT_GPU_API_NAME;
	};
#endif
}
