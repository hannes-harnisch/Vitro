module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"
export module Vitro.D3D12.Adapter;

import Vitro.D3D12.Utils;

namespace vt::d3d12
{
	export class Adapter
	{
	public:
		Adapter(ComUnique<IDXGIAdapter1> adapterHandle) : adapter(std::move(adapterHandle))
		{}

		IDXGIAdapter1* get() const
		{
			return adapter.get();
		}

	private:
		ComUnique<IDXGIAdapter1> adapter;
	};
}
