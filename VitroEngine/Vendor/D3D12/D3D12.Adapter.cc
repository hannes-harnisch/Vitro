module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"
export module vt.D3D12.Adapter;

import vt.D3D12.Utils;

namespace vt::d3d12
{
	export class D3D12Adapter
	{
	public:
		D3D12Adapter(ComUnique<IDXGIAdapter1> adapter_resource) : adapter(std::move(adapter_resource))
		{}

		IDXGIAdapter1* ptr() const
		{
			return adapter.get();
		}

	private:
		ComUnique<IDXGIAdapter1> adapter;
	};
}
