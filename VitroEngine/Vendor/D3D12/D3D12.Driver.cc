module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <array>
#include <vector>
export module Vitro.D3D12.Driver;

import Vitro.App.SharedLibrary;
import Vitro.D3D12.Unique;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DriverBase;
import Vitro.Windows.StringUtils;

namespace D3D12
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

		std::vector<::Adapter> enumerateAdapters() const override
		{
			std::vector<::Adapter> adapters;
			UINT index {};
			HRESULT result {};
			while(result != DXGI_ERROR_NOT_FOUND)
			{
				Unique<IDXGIAdapter> adapter;
				result = factory->EnumAdapters(index++, &adapter);
				adapters.emplace_back(std::move(adapter));
			}
			return adapters;
		}

		PFN_D3D12_CREATE_DEVICE getDeviceCreationFunction() const
		{
			return d3d12CreateDevice;
		}

		IDXGIFactory5* getFactory() const
		{
			return factory;
		}

		bool isSwapChainTearingAvailable() const
		{
			bool isAvailable;
			auto result = factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &isAvailable, sizeof isAvailable);
			vtEnsureResult(result, "Failed to check for swap chain tearing support.");
			return isAvailable;
		}

	private:
		SharedLibrary d3d12;
		SharedLibrary dxgi;
		PFN_D3D12_GET_DEBUG_INTERFACE d3d12GetDebugInterface;
		decltype(::CreateDXGIFactory2)* createDXGIFactory2;
		PFN_D3D12_CREATE_DEVICE d3d12CreateDevice;
#if VT_DEBUG
		Unique<ID3D12Debug> debug;
#endif
		Unique<IDXGIFactory5> factory;

		Unique<ID3D12Debug> makeDebugInterface()
		{
			Unique<ID3D12Debug> debugInterface;
			auto result = d3d12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
			vtEnsureResult(result, "Failed to get D3D12 debug interface.");

			debugInterface->EnableDebugLayer();
			return debugInterface;
		}

		Unique<IDXGIFactory5> makeFactory()
		{
			UINT flags = 0;
#if VT_DEBUG
			flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

			Unique<IDXGIFactory2> proxyFactory;
			auto result = createDXGIFactory2(flags, IID_PPV_ARGS(&proxyFactory));
			vtEnsureResult(result, "Failed to get proxy DXGI factory.");

			Unique<IDXGIFactory5> mainFactory;
			result = proxyFactory.queryFor(&mainFactory);
			vtEnsureResult(result, "Failed to get main DXGI factory.");
			return mainFactory;
		}
	};
}
