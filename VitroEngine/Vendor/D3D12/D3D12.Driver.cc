module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <array>
export module Vitro.D3D12.Driver;

import Vitro.Graphics.DriverBase;
import Vitro.D3D12.Unique;

namespace D3D12
{
	export class Driver final : public DriverBase
	{
	public:
		Driver() :
#if VT_DEBUG
			debug(makeDebugInterface()),
#endif
			factory(makeFactory())
		{}

	private:
#if VT_DEBUG
		Unique<ID3D12Debug> debug;
#endif
		Unique<IDXGIFactory2> factory;

		static ID3D12Debug* makeDebugInterface()
		{
			ID3D12Debug* debug;
			auto result = ::D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
			vtEnsureResult(result, "Failed to get D3D12 debug interface.");

			debug->EnableDebugLayer();
			return debug;
		}

		static IDXGIFactory2* makeFactory()
		{
			UINT flags = 0;
#if VT_DEBUG
			flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

			IDXGIFactory2* factory;
			auto result = ::CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory));
			vtEnsureResult(result, "Failed to get DXGI factory.");
			return factory;
		}
	};
}
