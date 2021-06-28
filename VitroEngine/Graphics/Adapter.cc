export module Vitro.Graphics.Adapter;

import Vitro.Graphics.AdapterBase;
import Vitro.Graphics.InterfaceObject;

#if VT_DYNAMIC_GHI

import Vitro.D3D12.Adapter;
import Vitro.Vulkan.Adapter;

export using Adapter = InterfaceObject<AdapterBase, D3D12::Adapter, Vulkan::Adapter>;

#else

import Vitro.VT_GHI.Adapter;

export using Adapter = InterfaceObject<AdapterBase, VT_GHI::Adapter>;

#endif
