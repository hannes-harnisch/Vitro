module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"
export module vt.D3D12.Adapter;

import vt.D3D12.Utils;

namespace vt::d3d12
{
	export class Adapter
	{
	public:
		Adapter(ComUnique<IDXGIAdapter1> adapter_resource) : adapter(std::move(adapter_resource))
		{}

		IDXGIAdapter1* get() const
		{
			return adapter.get();
		}

	private:
		ComUnique<IDXGIAdapter1> adapter;
	};
}
