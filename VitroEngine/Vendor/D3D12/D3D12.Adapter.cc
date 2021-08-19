module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <string_view>
export module Vitro.D3D12.Adapter;

import Vitro.D3D12.Utils;
import Vitro.Graphics.AdapterBase;
import Vitro.Windows.Utils;

namespace vt::d3d12
{
	export class Adapter final : public AdapterBase
	{
	public:
		Adapter(ComUnique<IDXGIAdapter1> adapterHandle) : adapter(std::move(adapterHandle))
		{
			DXGI_ADAPTER_DESC1 desc;

			auto result = adapter->GetDesc1(&desc);
			vtEnsureResult(result, "Failed to get D3D12 adapter info.");

			name = windows::narrowString(desc.Description);
			vram = desc.DedicatedVideoMemory;
		}

		IDXGIAdapter1* get()
		{
			return adapter.get();
		}

	private:
		ComUnique<IDXGIAdapter1> adapter;
	};
}
