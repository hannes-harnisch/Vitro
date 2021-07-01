export module Vitro.Graphics.CommandList;

import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GHI

import Vitro.D3D12.CommandList;
import Vitro.Vulkan.CommandList;

namespace vt
{
	export using CopyCommandList =
		InterfaceObject<CopyCommandListBase, D3D12::CommandList<QueuePurpose::Copy>, Vulkan::CommandList<QueuePurpose::Copy>>;

	export using ComputeCommandList = InterfaceObject<ComputeCommandListBase,
													  D3D12::CommandList<QueuePurpose::Compute>,
													  Vulkan::CommandList<QueuePurpose::Compute>>;

	export using GraphicsCommandList = InterfaceObject<GraphicsCommandListBase,
													   D3D12::CommandList<QueuePurpose::Graphics>,
													   Vulkan::CommandList<QueuePurpose::Graphics>>;
}

#else

import Vitro.VT_GHI_MODULE.CommandList;

namespace vt
{
	export using CopyCommandList = InterfaceObject<CopyCommandListBase, VT_GHI_NAME::CommandList<QueuePurpose::Copy>>;

	export using ComputeCommandList = InterfaceObject<ComputeCommandListBase, VT_GHI_NAME::CommandList<QueuePurpose::Compute>>;

	export using GraphicsCommandList =
		InterfaceObject<GraphicsCommandListBase, VT_GHI_NAME::CommandList<QueuePurpose::Graphics>>;
}

#endif
