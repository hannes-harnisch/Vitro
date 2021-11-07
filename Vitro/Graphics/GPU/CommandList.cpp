module;
#include "Core/Macros.hpp"
export module vt.Graphics.CommandList;

import vt.Graphics.CommandListBase;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.CommandList;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.CommandList;
#endif

namespace vt
{
	using PlatformCopyCommandList = InterfaceVariant<CopyCommandListBase,
													 VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Copy>)>;

	using PlatformComputeCommandList = InterfaceVariant<ComputeCommandListBase,
														VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Compute>)>;

	using PlatformRenderCommandList = InterfaceVariant<RenderCommandListBase,
													   VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Render>)>;

	// Use the arrow operator on it to record various copy commands.
	export class CopyCommandList : public PlatformCopyCommandList
	{
		using PlatformCopyCommandList::PlatformCopyCommandList;
	};

	// Use the arrow operator on it to record various compute commands.
	export class ComputeCommandList : public PlatformComputeCommandList
	{
		using PlatformComputeCommandList::PlatformComputeCommandList;
	};

	// Use the arrow operator on it to record various rendering commands.
	export class RenderCommandList : public PlatformRenderCommandList
	{
		using PlatformRenderCommandList::PlatformRenderCommandList;
	};
}
