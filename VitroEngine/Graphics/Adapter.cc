export module Vitro.Graphics.Adapter;

import Vitro.Graphics.AdapterBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GRAPHICS_API
import Vitro.VT_GRAPHICS_API_MODULE_PRIMARY.Adapter;
#endif
import Vitro.VT_GRAPHICS_API_MODULE.Adapter;

namespace vt
{
	export using Adapter = InterfaceVariant<AdapterBase,
#if VT_DYNAMIC_GRAPHICS_API
											VT_GRAPHICS_API_NAME_PRIMARY::Adapter,
#endif
											VT_GRAPHICS_API_NAME::Adapter>;
}
