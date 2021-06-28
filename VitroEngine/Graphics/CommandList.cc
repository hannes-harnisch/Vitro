export module Vitro.Graphics.CommandList;

import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GHI

import Vitro.D3D12.CommandList;
import Vitro.Vulkan.CommandList;

namespace vt
{
	export using CommandList = InterfaceObject<CommandListBase, D3D12::CommandList, Vulkan::CommandList>;
}

#else

import Vitro.VT_GHI_MODULE.CommandList;

namespace vt
{
	export using CommandList = InterfaceObject<CommandListBase, VT_GHI_NAME::CommandList>;
}

#endif
