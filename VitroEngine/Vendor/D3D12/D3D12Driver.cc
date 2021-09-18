module;
#include "Core/Macros.hh"
#include "D3D12API.hh"

#include <array>
#include <string_view>
#include <type_traits>
#include <vector>
export module vt.D3D12.Driver;

import vt.Core.Version;
import vt.D3D12.Handle;
import vt.Graphics.DriverBase;
import vt.Windows.Utils;

namespace vt::d3d12
{
	export class D3D12Driver final : public DriverBase
	{
	public:
		static constexpr D3D_FEATURE_LEVEL MinimumFeatureLevel = D3D_FEATURE_LEVEL_11_1;

		// The unused parameters are kept for compatibility with Vulkan, which takes an application name and versions.
		D3D12Driver(std::string const&, Version, Version)
		{
#if VT_DEBUG
			initialize_debug_interface();
#endif
			initialize_factory();
		}

		std::vector<Adapter> enumerate_adapters() const override
		{
			std::vector<Adapter> adapters;
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

				auto can_make_device = D3D12CreateDevice(adapter.get(), MinimumFeatureLevel, __uuidof(ID3D12Device4), nullptr);
				if(FAILED(can_make_device) || desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					continue;

				adapters.emplace_back(std::move(adapter), windows::narrow_string(desc.Description), desc.DedicatedVideoMemory);
			}
			return adapters;
		}

		IDXGIFactory5* get_factory() const
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
		ComUnique<ID3D12Debug>	 debug;
		ComUnique<IDXGIFactory5> factory;

		void initialize_debug_interface()
		{
			ID3D12Debug* raw_debug;

			auto result = D3D12GetDebugInterface(IID_PPV_ARGS(&raw_debug));
			debug.reset(raw_debug);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 debug interface.");

			debug->EnableDebugLayer();
		}

		void initialize_factory()
		{
#if VT_DEBUG
			UINT flags = DXGI_CREATE_FACTORY_DEBUG;
#else
			UINT flags = 0;
#endif
			IDXGIFactory5* raw_factory;

			auto result = CreateDXGIFactory2(flags, IID_PPV_ARGS(&raw_factory));
			factory.reset(raw_factory);
			VT_ENSURE_RESULT(result, "Failed to create DXGI factory.");
		}
	};
}
