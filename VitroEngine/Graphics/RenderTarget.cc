export module Vitro.Graphics.RenderTarget;

import Vitro.Graphics.InterfaceVariant;
import Vitro.Graphics.RenderTargetInfo;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.RenderTarget;
#endif
import Vitro.VT_GPU_API_MODULE.RenderTarget;

namespace vt
{
	export using RenderTarget = InterfaceVariant<RenderTargetBase,
#if VT_DYNAMIC_GPU_API
												 VT_GPU_API_NAME_PRIMARY::RenderTarget,
#endif
												 VT_GPU_API_NAME::RenderTarget>;
}
