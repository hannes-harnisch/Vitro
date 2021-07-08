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
		Device(vt::Driver const& driver, vt::Adapter const& adapter) :
			device(makeDevice(driver, adapter)),
			graphicsQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT),
			computeQueue(device, D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copyQueue(device, D3D12_COMMAND_LIST_TYPE_COPY)
		{}

		ID3D12Device* getDevice()
		{
			return device;
		}

		ID3D12CommandQueue* getGraphicsQueue()
		{
			return graphicsQueue.getHandle();
		}

		ID3D12CommandQueue* getComputeQueue()
		{
			return computeQueue.getHandle();
		}

		ID3D12CommandQueue* getCopyQueue()
		{
			return copyQueue.getHandle();
		}

	private:
		constexpr static D3D_FEATURE_LEVEL TargetFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		ComUnique<ID3D12Device> device;
		Queue graphicsQueue;
		Queue computeQueue;
		Queue copyQueue;

		static ComUnique<ID3D12Device> makeDevice(vt::Driver const& driver, vt::Adapter const& adapter)
		{
			auto const d3d12CreateDevice = driver.d3d12.getDeviceCreationFunction();
			ComUnique<ID3D12Device> device;
			auto result = d3d12CreateDevice(adapter.d3d12.getHandle(), TargetFeatureLevel, IID_PPV_ARGS(&device));
			vtEnsureResult(result, "Failed to create D3D12 device.");
			return device;
		}
	};
}
