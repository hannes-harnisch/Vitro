export module Vitro.Graphics.CommandList;

import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GHI

import Vitro.D3D12.CommandList;
import Vitro.Vulkan.CommandList;

export using CommandList = InterfaceObject<CommandListBase, D3D12::CommandList, Vulkan::CommandList>;

#else

import Vitro.VT_GHI.CommandList;

export using CommandList = InterfaceObject<CommandListBase, VT_GHI::CommandList>;

#endif
