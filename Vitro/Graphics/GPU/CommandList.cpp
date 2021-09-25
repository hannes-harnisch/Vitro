module;
#include "Core/Macros.hpp"
export module vt.Graphics.CommandList;

import vt.Graphics.CommandListBase;
import vt.Graphics.Device;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.VT_GPU_API_MODULE.CommandList;

#if VT_DYNAMIC_GPU_API
import vt.Graphics.VT_GPU_API_MODULE_SECONDARY.CommandList;
#endif

namespace vt
{
	using PlatformCopyCommandList = InterfaceVariant<CopyCommandListBase,
													 VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Copy>)>;
	export class CopyCommandList : public PlatformCopyCommandList
	{
	public:
		CopyCommandList(Device& device) : PlatformCopyCommandList(device)
		{}
	};

	using PlatformComputeCommandList = InterfaceVariant<ComputeCommandListBase,
														VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Compute>)>;
	export class ComputeCommandList : public PlatformComputeCommandList
	{
	public:
		ComputeCommandList(Device& device) : PlatformComputeCommandList(device)
		{}
	};

	using PlatformRenderCommandList = InterfaceVariant<RenderCommandListBase,
													   VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Render>)>;
	export class RenderCommandList : public PlatformRenderCommandList
	{
	public:
		RenderCommandList(Device& device) : PlatformRenderCommandList(device)
		{}
	};
}
