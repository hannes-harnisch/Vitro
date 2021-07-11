module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <array>
#include <vector>
export module Vitro.D3D12.Driver;

import Vitro.App.SharedLibrary;
import Vitro.D3D12.ComUnique;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DriverBase;
import Vitro.Windows.StringUtils;

namespace vt::d3d12
{
	export class Driver final : public DriverBase
	{
	public:
		Driver() :
			d3d12("d3d12"),
			dxgi("dxgi"),
			d3d12GetDebugInterface(d3d12.loadSymbol<decltype(::D3D12GetDebugInterface)>("D3D12GetDebugInterface")),
			createDXGIFactory2(dxgi.loadSymbol<decltype(::CreateDXGIFactory2)>("CreateDXGIFactory2")),
			d3d12CreateDevice(d3d12.loadSymbol<decltype(::D3D12CreateDevice)>("D3D12CreateDevice")),
#if VT_DEBUG
			debug(makeDebugInterface()),
#endif
			factory(makeFactory())
		{}

		std::vector<vt::Adapter> enumerateAdapters() const override
		{
			std::vector<vt::Adapter> adapters;
			for(UINT index = 0;; ++index)
			{
				ComUnique<IDXGIAdapter> adapter;
				auto result = factory->EnumAdapters(index, &adapter);
				if(result == DXGI_ERROR_NOT_FOUND)
					break;

				adapters.emplace_back(std::move(adapter));
			}
			return adapters;
		}

		PFN_D3D12_CREATE_DEVICE deviceCreationFunction() const
		{
			return d3d12CreateDevice;
		}

		IDXGIFactory5* handle() const
		{
			return factory;
		}

		bool swapChainTearingAvailable() const
		{
			BOOL available;
			auto result = factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &available, sizeof available);
			vtEnsureResult(result, "Failed to check for swap chain tearing support.");
			return available;
		}

	private:
		SharedLibrary d3d12;
		SharedLibrary dxgi;
		PFN_D3D12_GET_DEBUG_INTERFACE d3d12GetDebugInterface;
		decltype(::CreateDXGIFactory2)* createDXGIFactory2;
		PFN_D3D12_CREATE_DEVICE d3d12CreateDevice;
#if VT_DEBUG
		ComUnique<ID3D12Debug> debug;
#endif
		ComUnique<IDXGIFactory5> factory;

		ComUnique<ID3D12Debug> makeDebugInterface()
		{
			ComUnique<ID3D12Debug> debugInterface;
			auto result = d3d12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
			vtEnsureResult(result, "Failed to get D3D12 debug interface.");

			debugInterface->EnableDebugLayer();
			return debugInterface;
		}

		ComUnique<IDXGIFactory5> makeFactory()
		{
			UINT flags = 0;
#if VT_DEBUG
			flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

			ComUnique<IDXGIFactory2> proxyFactory;
			auto result = createDXGIFactory2(flags, IID_PPV_ARGS(&proxyFactory));
			vtEnsureResult(result, "Failed to get proxy DXGI factory.");

			ComUnique<IDXGIFactory5> mainFactory;
			result = proxyFactory.queryFor(&mainFactory);
			vtEnsureResult(result, "Failed to get main DXGI factory.");
			return mainFactory;
		}
	};
}
