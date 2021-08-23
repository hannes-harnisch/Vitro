module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <array>
#include <string_view>
#include <type_traits>
#include <vector>
export module Vitro.D3D12.Driver;

import Vitro.App.SharedLibrary;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DriverBase;
import Vitro.Windows.Utils;

namespace vt::d3d12
{
	export using CommandListHandle = ID3D12CommandList*;

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

		std::vector<vt::Adapter> enumerateAdapters() const override
		{
			std::vector<vt::Adapter> adapters;
			for(unsigned index = 0;; ++index)
			{
				IDXGIAdapter1* rawAdapter;

				auto result = factory->EnumAdapters1(index, &rawAdapter);
				if(result == DXGI_ERROR_NOT_FOUND)
					break;
				ComUnique<IDXGIAdapter1> adapter(rawAdapter);

				DXGI_ADAPTER_DESC1 desc;
				result = adapter->GetDesc1(&desc);
				vtEnsureResult(result, "Failed to get D3D12 adapter info.");

				bool cantMakeDevice = FAILED(d3d12CreateDevice(adapter.get(), FeatureLevel, __uuidof(ID3D12Device4), nullptr));
				if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || cantMakeDevice)
					continue;

				adapters.emplace_back(std::move(adapter), windows::narrowString(desc.Description), desc.DedicatedVideoMemory);
			}
			return adapters;
		}

		template<typename T> T loadD3D12Function(std::string_view name) const
		{
			return d3d12.loadSymbol<std::remove_pointer_t<T>>(name);
		}

		IDXGIFactory5* get() const
		{
			return factory.get();
		}

		bool swapChainTearingSupported() const
		{
			BOOL supported;
			auto result = factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof supported);
			vtEnsureResult(result, "Failed to check for swap chain tearing support.");
			return supported;
		}

	private:
		SharedLibrary d3d12;
		SharedLibrary dxgi;

	public:
		PFN_D3D12_GET_DEBUG_INTERFACE const	  d3d12GetDebugInterface;
		decltype(::CreateDXGIFactory2)* const createDXGIFactory2;
		PFN_D3D12_CREATE_DEVICE const		  d3d12CreateDevice;

	private:
		ComUnique<ID3D12Debug>	 debug;
		ComUnique<IDXGIFactory5> factory;

		decltype(debug) makeDebugInterface()
		{
			ID3D12Debug* rawDebug;

			auto result = d3d12GetDebugInterface(IID_PPV_ARGS(&rawDebug));

			decltype(debug) freshDebug(rawDebug);
			vtEnsureResult(result, "Failed to create D3D12 debug interface.");

			freshDebug->EnableDebugLayer();
			return freshDebug;
		}

		decltype(factory) makeFactory()
		{
#if VT_DEBUG
			UINT flags = DXGI_CREATE_FACTORY_DEBUG;
#else
			UINT flags = 0;
#endif
			IDXGIFactory5* rawFactory;

			auto result = createDXGIFactory2(flags, IID_PPV_ARGS(&rawFactory));

			decltype(factory) freshFactory(rawFactory);
			vtEnsureResult(result, "Failed to create DXGI factory.");

			return freshFactory;
		}
	};
}
