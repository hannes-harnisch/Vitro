export module Vitro.Graphics.Adapter;

import Vitro.Graphics.AdapterBase;
import Vitro.Graphics.InterfaceVariant;

#if VT_DYNAMIC_GRAPHICS_API

import Vitro.D3D12.Adapter;
import Vitro.Vulkan.Adapter;

namespace vt
{
	export using Adapter = InterfaceVariant<AdapterBase, D3D12::Adapter, Vulkan::Adapter>;
}

#else

import Vitro.VT_GRAPHICS_API_MODULE.Adapter;

namespace vt
{
	export using Adapter = InterfaceVariant<AdapterBase, VT_GRAPHICS_API_NAME::Adapter>;
}

#endif
