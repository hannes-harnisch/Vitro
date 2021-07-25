module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <array>
#include <vector>
export module Vitro.D3D12.Driver;

import Vitro.App.SharedLibrary;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DriverBase;
import Vitro.Windows.StringUtils;

namespace vt::d3d12
{
	export class Driver final : public DriverBase
	{
	public:
		static constexpr D3D_FEATURE_LEVEL FeatureLevel = D3D_FEATURE_LEVEL_11_0;

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

		std::vector<vt::Adapter> enumerateAdapters() override
		{
			std::vector<vt::Adapter> adapters;
			for(UINT index = 0;; ++index)
			{
				IDXGIAdapter1* adapterPtr;
				auto result = factory->EnumAdapters1(index, &adapterPtr);
				ComUnique<IDXGIAdapter1> adapter(adapterPtr);
				if(result == DXGI_ERROR_NOT_FOUND)
					break;

				DXGI_ADAPTER_DESC1 desc;
				result = adapter->GetDesc1(&desc);
				vtEnsureResult(result, "Failed to get D3D12 adapter info.");

				bool cannotMakeDevice = FAILED(d3d12CreateDevice(adapter.get(), FeatureLevel, __uuidof(ID3D12Device), nullptr));
				if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || cannotMakeDevice)
					continue;

				adapters.emplace_back(std::move(adapter));
			}
			return adapters;
		}

		PFN_D3D12_CREATE_DEVICE deviceCreationFunction()
		{
			return d3d12CreateDevice;
		}

		IDXGIFactory5* handle()
		{
			return factory.get();
		}

		bool swapChainTearingAvailable()
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

		ID3D12Debug* makeDebugInterface()
		{
			ID3D12Debug* debugInterface;
			auto result = d3d12GetDebugInterface(IID_PPV_ARGS(&debugInterface));
			vtEnsureResult(result, "Failed to get D3D12 debug interface.");

			debugInterface->EnableDebugLayer();
			return debugInterface;
		}

		IDXGIFactory5* makeFactory()
		{
			UINT flags = 0;
#if VT_DEBUG
			flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

			IDXGIFactory2* factoryPtr;
			auto result = createDXGIFactory2(flags, IID_PPV_ARGS(&factoryPtr));
			ComUnique<IDXGIFactory2> proxyFactory(factoryPtr);
			vtEnsureResult(result, "Failed to get proxy DXGI factory.");

			IDXGIFactory5* mainFactory;
			result = proxyFactory->QueryInterface(&mainFactory);
			vtEnsureResult(result, "Failed to get main DXGI factory.");
			return mainFactory;
		}
	};
}
