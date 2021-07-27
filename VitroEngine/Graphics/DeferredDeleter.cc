export module Vitro.Graphics.DeferredDeleter;

import Vitro.Core.Singleton;
import Vitro.Graphics.DeferredDeleterBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.DeferredDeleter;
#endif
import Vitro.VT_GPU_API_MODULE.DeferredDeleter;

namespace vt
{
	export struct DeferredDeleter :
		InterfaceVariant<DeferredDeleterBase,
#if VT_DYNAMIC_GPU_API
						 VT_GPU_API_NAME_PRIMARY::DeferredDeleter,
#endif
						 VT_GPU_API_NAME::DeferredDeleter>,
		Singleton<DeferredDeleter>
	{
		using InterfaceVariant<DeferredDeleterBase,
#if VT_DYNAMIC_GPU_API
							   VT_GPU_API_NAME_PRIMARY::DeferredDeleter,
#endif
							   VT_GPU_API_NAME::DeferredDeleter>::InterfaceVariant;
	};
}
