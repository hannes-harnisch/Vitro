export module Vitro.Graphics.CommandList;

import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.CommandList;
import Vitro.Vulkan.CommandList;

namespace vt
{
	export using CopyCommandList =
		InterfaceVariant<CopyCommandListBase, D3D12::CommandList<QueuePurpose::Copy>, Vulkan::CommandList<QueuePurpose::Copy>>;

	export using ComputeCommandList = InterfaceVariant<ComputeCommandListBase,
													   D3D12::CommandList<QueuePurpose::Compute>,
													   Vulkan::CommandList<QueuePurpose::Compute>>;

	export using GraphicsCommandList = InterfaceVariant<GraphicsCommandListBase,
														D3D12::CommandList<QueuePurpose::Graphics>,
														Vulkan::CommandList<QueuePurpose::Graphics>>;
}

#else

import Vitro.VT_GRAPHICS_API_MODULE.CommandList;

namespace vt
{
	export using CopyCommandList = InterfaceVariant<CopyCommandListBase, VT_GRAPHICS_API_NAME::CommandList<QueuePurpose::Copy>>;

	export using ComputeCommandList =
		InterfaceVariant<ComputeCommandListBase, VT_GRAPHICS_API_NAME::CommandList<QueuePurpose::Compute>>;

	export using GraphicsCommandList =
		InterfaceVariant<GraphicsCommandListBase, VT_GRAPHICS_API_NAME::CommandList<QueuePurpose::Graphics>>;
}

#endif
