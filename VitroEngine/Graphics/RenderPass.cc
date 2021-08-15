export module Vitro.Graphics.RenderPass;

import Vitro.Graphics.InterfaceVariant;
import Vitro.Graphics.RenderPassInfo;

#if VT_DYNAMIC_GPU_API
import Vitro.VT_GPU_API_MODULE_PRIMARY.RenderPass;
#endif
import Vitro.VT_GPU_API_MODULE.RenderPass;

namespace vt
{
	export using RenderPass = InterfaceVariant<RenderPassBase,
#if VT_DYNAMIC_GPU_API
											   VT_GPU_API_NAME_PRIMARY::RenderPass,
#endif
											   VT_GPU_API_NAME::RenderPass>;
}
