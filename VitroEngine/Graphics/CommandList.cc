export module vt.Graphics.CommandList;

import vt.Graphics.CommandListBase;
import vt.Graphics.DynamicGpuApi;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_PRIMARY.CommandList;
#endif
import vt.VT_GPU_API_MODULE.CommandList;

namespace vt
{
	export using CopyCommandList = InterfaceVariant<CopyCommandListBase,
#if VT_DYNAMIC_GPU_API
													VT_GPU_API_NAME_PRIMARY::CommandList<CommandType::Copy>,
#endif
													VT_GPU_API_NAME::CommandList<CommandType::Copy>>;

	export using ComputeCommandList = InterfaceVariant<ComputeCommandListBase,
#if VT_DYNAMIC_GPU_API
													   VT_GPU_API_NAME_PRIMARY::CommandList<CommandType::Compute>,
#endif
													   VT_GPU_API_NAME::CommandList<CommandType::Compute>>;

	export using RenderCommandList = InterfaceVariant<RenderCommandListBase,
#if VT_DYNAMIC_GPU_API
													  VT_GPU_API_NAME_PRIMARY::CommandList<CommandType::Render>,
#endif
													  VT_GPU_API_NAME::CommandList<CommandType::Render>>;
}
