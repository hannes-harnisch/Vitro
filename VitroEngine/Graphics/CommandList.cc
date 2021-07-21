export module Vitro.Graphics.CommandList;

import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.CommandType;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GRAPHICS_API
import Vitro.VT_GRAPHICS_API_MODULE_PRIMARY.CommandList;
#endif
import Vitro.VT_GRAPHICS_API_MODULE.CommandList;

namespace vt
{
	export using CopyCommandList = InterfaceVariant<CopyCommandListBase,
#if VT_DYNAMIC_GRAPHICS_API
													VT_GRAPHICS_API_NAME_PRIMARY::CommandList<CommandType::Copy>,
#endif
													VT_GRAPHICS_API_NAME::CommandList<CommandType::Copy>>;

	export using ComputeCommandList = InterfaceVariant<ComputeCommandListBase,
#if VT_DYNAMIC_GRAPHICS_API
													   VT_GRAPHICS_API_NAME_PRIMARY::CommandList<CommandType::Compute>,
#endif
													   VT_GRAPHICS_API_NAME::CommandList<CommandType::Compute>>;

	export using RenderCommandList = InterfaceVariant<RenderCommandListBase,
#if VT_DYNAMIC_GRAPHICS_API
													  VT_GRAPHICS_API_NAME_PRIMARY::CommandList<CommandType::Render>,
#endif
													  VT_GRAPHICS_API_NAME::CommandList<CommandType::Render>>;
}
