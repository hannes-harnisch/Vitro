module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.Device;

import Vitro.Core.Unique;
import Vitro.D3D12.ComUnique;
import Vitro.D3D12.Queue;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.Driver;
import Vitro.Graphics.Handle;
import Vitro.D3D12.Queue;

namespace vt::d3d12
{
	export class Device final : public DeviceBase
	{
	public:
		Device(vt::Driver& driver, vt::Adapter adapter) :
			device(makeDevice(driver, adapter)),
			renderQ(device, D3D12_COMMAND_LIST_TYPE_DIRECT),
			computeQ(device, D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copyQ(device, D3D12_COMMAND_LIST_TYPE_COPY)
		{}

		ID3D12Device* handle()
		{
			return device;
		}

		ID3D12CommandQueue* renderQueue()
		{
			return renderQ.handle();
		}

		ID3D12CommandQueue* computeQueue()
		{
			return computeQ.handle();
		}

		ID3D12CommandQueue* copyQueue()
		{
			return copyQ.handle();
		}

	private:
		constexpr static D3D_FEATURE_LEVEL TargetFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		ComUnique<ID3D12Device> device;
		Queue renderQ;
		Queue computeQ;
		Queue copyQ;

		static ComUnique<ID3D12Device> makeDevice(vt::Driver& driver, vt::Adapter& adapter)
		{
			auto const d3d12CreateDevice = driver.d3d12.deviceCreationFunction();
			ComUnique<ID3D12Device> device;
			auto result = d3d12CreateDevice(adapter.d3d12.handle(), TargetFeatureLevel, IID_PPV_ARGS(&device));
			vtEnsureResult(result, "Failed to create D3D12 device.");
			return device;
		}
	};
}
