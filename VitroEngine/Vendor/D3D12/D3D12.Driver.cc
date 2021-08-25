module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <array>
#include <string_view>
#include <type_traits>
#include <vector>
export module vt.D3D12.Driver;

import vt.App.SharedLibrary;
import vt.D3D12.Utils;
import vt.Graphics.Adapter;
import vt.Graphics.DriverBase;
import vt.Windows.Utils;

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
			d3d12_get_debug_interface(d3d12.load_symbol<decltype(D3D12GetDebugInterface)>("D3D12GetDebugInterface")),
			create_dxgi_factory2(dxgi.load_symbol<decltype(CreateDXGIFactory2)>("CreateDXGIFactory2")),
			d3d12_create_device(d3d12.load_symbol<decltype(D3D12CreateDevice)>("D3D12CreateDevice")),
#if VT_DEBUG
			debug(make_debug_interface()),
#endif
			factory(make_factory())
		{}

		std::vector<vt::Adapter> enumerate_adapters() const override
		{
			std::vector<vt::Adapter> adapters;
			for(unsigned index = 0;; ++index)
			{
				IDXGIAdapter1* raw_adapter;

				auto result = factory->EnumAdapters1(index, &raw_adapter);
				if(result == DXGI_ERROR_NOT_FOUND)
					break;
				ComUnique<IDXGIAdapter1> adapter(raw_adapter);

				DXGI_ADAPTER_DESC1 desc;
				result = adapter->GetDesc1(&desc);
				VT_ENSURE_RESULT(result, "Failed to get D3D12 adapter info.");

				bool has_no_device = FAILED(d3d12_create_device(adapter.get(), FeatureLevel, __uuidof(ID3D12Device4), nullptr));
				if(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || has_no_device)
					continue;

				adapters.emplace_back(std::move(adapter), windows::narrow_string(desc.Description), desc.DedicatedVideoMemory);
			}
			return adapters;
		}

		template<typename T> T load_d3d12_function(std::string_view name) const
		{
			return d3d12.load_symbol<std::remove_pointer_t<T>>(name);
		}

		IDXGIFactory5* get() const
		{
			return factory.get();
		}

		bool swap_chain_tearing_supported() const
		{
			BOOL supported;
			auto result = factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof supported);
			VT_ENSURE_RESULT(result, "Failed to check for swap chain tearing support.");
			return supported;
		}

	private:
		SharedLibrary d3d12;
		SharedLibrary dxgi;

	public:
		PFN_D3D12_GET_DEBUG_INTERFACE const d3d12_get_debug_interface;
		decltype(CreateDXGIFactory2)* const create_dxgi_factory2;
		PFN_D3D12_CREATE_DEVICE const		d3d12_create_device;

	private:
		ComUnique<ID3D12Debug>	 debug;
		ComUnique<IDXGIFactory5> factory;

		decltype(debug) make_debug_interface()
		{
			ID3D12Debug* raw_debug;

			auto result = d3d12_get_debug_interface(IID_PPV_ARGS(&raw_debug));

			decltype(debug) fresh_debug(raw_debug);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 debug interface.");

			fresh_debug->EnableDebugLayer();
			return fresh_debug;
		}

		decltype(factory) make_factory()
		{
#if VT_DEBUG
			UINT flags = DXGI_CREATE_FACTORY_DEBUG;
#else
			UINT flags = 0;
#endif
			IDXGIFactory5* raw_factory;

			auto result = create_dxgi_factory2(flags, IID_PPV_ARGS(&raw_factory));

			decltype(factory) fresh_factory(raw_factory);
			VT_ENSURE_RESULT(result, "Failed to create DXGI factory.");

			return fresh_factory;
		}
	};
}
