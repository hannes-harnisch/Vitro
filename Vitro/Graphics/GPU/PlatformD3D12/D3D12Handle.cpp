module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"
export module vt.Graphics.D3D12.Handle;

import vt.Core.Windows.Utils;

namespace vt::d3d12
{
	export using windows::ComUnique;

	export using D3D12Adapter			= ComUnique<IDXGIAdapter1>;
	export using D3D12CommandListHandle = ID3D12CommandList*;

	export struct D3D12SyncToken
	{
		ID3D12Fence* fence		 = nullptr;
		uint64_t	 fence_value = 0;
	};
}
