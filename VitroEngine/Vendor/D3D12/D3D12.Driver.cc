module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <array>
#include <vector>
export module Vitro.D3D12.Driver;

import Vitro.App.SharedLibrary;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DriverBase;
import Vitro.D3D12.Unique;
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

		std::vector<Adapter> enumerateAdapters() const override
		{
			IDXGIAdapter1* adapter;
			std::vector<Adapter> adapters;
			for(UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				DXGI_ADAPTER_DESC1 desc;
				adapter->GetDesc1(&desc);

				adapters.emplace_back(adapter, Windows::narrowString(desc.Description), desc.DedicatedVideoMemory);
			}
			return adapters;
		}

	private:
		SharedLibrary d3d12;
		SharedLibrary dxgi;
		decltype(::D3D12GetDebugInterface)* d3d12GetDebugInterface;
		decltype(::CreateDXGIFactory2)* createDXGIFactory2;
		decltype(::D3D12CreateDevice)* d3d12CreateDevice;
#if VT_DEBUG
		Unique<ID3D12Debug> debug;
#endif
		Unique<IDXGIFactory5> factory;

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

			Unique<IDXGIFactory2> proxyFactory;
			auto result = createDXGIFactory2(flags, IID_PPV_ARGS(&proxyFactory));
			vtEnsureResult(result, "Failed to get proxy DXGI factory.");

			IDXGIFactory5* mainFactory;
			vtEnsureResult(proxyFactory.queryFor(&mainFactory), "Failed to get main DXGI factory.");
			return mainFactory;
		}
	};
}
