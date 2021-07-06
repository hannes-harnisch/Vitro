module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <string_view>
export module Vitro.D3D12.Adapter;

import Vitro.D3D12.Unique;
import Vitro.Graphics.AdapterBase;
import Vitro.Windows.StringUtils;

namespace vt::d3d12
{
	export class Adapter final : public AdapterBase
	{
	public:
		Adapter(Unique<IDXGIAdapter> handle) : adapter(std::move(handle))
		{
			DXGI_ADAPTER_DESC desc;
			auto result = adapter->GetDesc(&desc);
			vtEnsureResult(result, "Failed to get D3D12 adapter info.");

			name = windows::narrowString(desc.Description);
			vram = desc.DedicatedVideoMemory;
		}

		std::string_view getName() const override
		{
			return name;
		}

		size_t getMemory() const override
		{
			return vram;
		}

		IDXGIAdapter* getHandle() const
		{
			return adapter;
		}

	private:
		Unique<IDXGIAdapter> adapter;
		std::string name;
		size_t vram;
	};
}
