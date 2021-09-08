module;
#include "Core/Macros.hh"
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
													VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Copy>)>;

	export using ComputeCommandList = InterfaceVariant<ComputeCommandListBase,
													   VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Compute>)>;

	export using RenderCommandList = InterfaceVariant<RenderCommandListBase,
													  VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Render>)>;
}
