module;
#include "Core/Macros.hh"
export module vt.Graphics.CommandList;

import vt.Graphics.CommandListBase;
import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.CommandList;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.CommandList;
#endif

namespace vt
{
	export using CopyCommandList = InterfaceVariant<CopyCommandListBase,
													VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Copy>)>;

	export using ComputeCommandList = InterfaceVariant<ComputeCommandListBase,
													   VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Compute>)>;

	export using RenderCommandList = InterfaceVariant<RenderCommandListBase,
													  VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Render>)>;
}
