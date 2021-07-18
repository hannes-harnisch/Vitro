module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <string_view>
export module Vitro.D3D12.Adapter;

import Vitro.D3D12.ComUnique;
import Vitro.Graphics.AdapterBase;
import Vitro.Windows.StringUtils;

namespace vt::d3d12
{
	export class Adapter final : public AdapterBase
	{
	public:
		Adapter(ComUnique<IDXGIAdapter> adapterHandle) : adapter(std::move(adapterHandle))
		{
			DXGI_ADAPTER_DESC desc;
			auto result = adapter->GetDesc(&desc);
			vtEnsureResult(result, "Failed to get D3D12 adapter info.");

			name = windows::narrowString(desc.Description);
			vram = desc.DedicatedVideoMemory;
		}

		IDXGIAdapter* handle()
		{
			return adapter;
		}

	private:
		ComUnique<IDXGIAdapter> adapter;
	};
}
