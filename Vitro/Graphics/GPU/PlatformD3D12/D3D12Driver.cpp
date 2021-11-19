module;
#include "D3D12API.hpp"
#include "VitroCore/Macros.hpp"

#include <string_view>
#include <type_traits>
#include <vector>
module vt.Graphics.D3D12.Driver;

import vt.Core.Version;
import vt.Core.Windows.Utils;
import vt.Graphics.D3D12.Device;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.AbstractDriver;

namespace vt::d3d12
{
	D3D12Driver::D3D12Driver(bool enable_debug_layer, std::string const&, Version, Version)
	{
		UINT factory_flags;

		if(enable_debug_layer)
		{
			auto result = D3D12GetDebugInterface(VT_COM_OUT(debug));
			VT_CHECK_RESULT(result, "Failed to create D3D12 debug interface.");

			debug->EnableDebugLayer();
			factory_flags = DXGI_CREATE_FACTORY_DEBUG;
		}
		else
			factory_flags = 0;

		auto result = CreateDXGIFactory2(factory_flags, VT_COM_OUT(factory));
		VT_CHECK_RESULT(result, "Failed to create DXGI factory.");
	}

	SmallList<Adapter> D3D12Driver::enumerate_adapters() const 
	{
		SmallList<Adapter> adapters;
		for(UINT index = 0;; ++index)
		{
			ComUnique<IDXGIAdapter1> adapter;

			auto result = factory->EnumAdapters1(index, std::out_ptr(adapter));
			if(result == DXGI_ERROR_NOT_FOUND)
				break;

			DXGI_ADAPTER_DESC1 desc;
			result = adapter->GetDesc1(&desc);
			VT_CHECK_RESULT(result, "Failed to get D3D12 adapter description.");

			auto can_make_device = D3D12CreateDevice(adapter.get(), D3D12Device::MIN_FEATURE_LEVEL, __uuidof(ID3D12Device4),
													 nullptr);
			if(FAILED(can_make_device) || desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				continue;

			adapters.emplace_back(std::move(adapter), windows::narrow_string(desc.Description), desc.DedicatedVideoMemory);
		}
		return adapters;
	}

	Device D3D12Driver::make_device(Adapter const& adapter) const 
	{
		return D3D12Device(adapter, *factory);
	}
}
