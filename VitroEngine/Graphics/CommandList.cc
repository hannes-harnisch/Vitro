module;
#include "Core/Macros.hh"
export module vt.Graphics.CommandList;

import vt.Graphics.CommandListBase;
import vt.Graphics.Device;
import vt.Graphics.DynamicGpuApi;
import vt.VT_GPU_API_MODULE.CommandList;

#if VT_DYNAMIC_GPU_API
import vt.VT_GPU_API_MODULE_SECONDARY.CommandList;
#endif

namespace vt
{
	using PlatformCopyCommandList = InterfaceVariant<CopyCommandListBase,
													 VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Copy>)>;
	export class CopyCommandList : public PlatformCopyCommandList
	{
	public:
		CopyCommandList(Device const& device) : PlatformCopyCommandList(device)
		{}
	};

	using PlatformComputeCommandList = InterfaceVariant<ComputeCommandListBase,
														VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Compute>)>;
	export class ComputeCommandList : public PlatformComputeCommandList
	{
	public:
		ComputeCommandList(Device const& device) : PlatformComputeCommandList(device)
		{}
	};

	using PlatformRenderCommandList = InterfaceVariant<RenderCommandListBase,
													   VT_GPU_API_VARIANT_ARGS(CommandList<CommandType::Render>)>;
	export class RenderCommandList : public PlatformRenderCommandList
	{
	public:
		RenderCommandList(Device const& device) : PlatformRenderCommandList(device)
		{}
	};
}
