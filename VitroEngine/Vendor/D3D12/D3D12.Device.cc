module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <span>
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
			renderQueue(device, D3D12_COMMAND_LIST_TYPE_DIRECT),
			computeQueue(device, D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copyQueue(device, D3D12_COMMAND_LIST_TYPE_COPY)
		{}

		void submitCopyCommands(std::span<vt::CommandListHandle const> commands) override
		{
#if VT_DEBUG
			for(auto handle : commands)
				vtAssert(handle.d3d12.commandType == CommandType::Copy,
						 "All command lists for this submission must be copy command lists.");
#endif
			copyQueue.submit(commands);
		}

		void submitComputeCommands(std::span<vt::CommandListHandle const> commands) override
		{
#if VT_DEBUG
			for(auto handle : commands)
				vtAssert(handle.d3d12.commandType == CommandType::Compute,
						 "All command lists for this submission must be compute command lists.");
#endif
			computeQueue.submit(commands);
		}

		void submitRenderCommands(std::span<vt::CommandListHandle const> commands) override
		{
#if VT_DEBUG
			for(auto handle : commands)
				vtAssert(handle.d3d12.commandType == CommandType::Render,
						 "All command lists for this submission must be render command lists.");
#endif
			renderQueue.submit(commands);
		}

		ID3D12Device1* handle()
		{
			return device;
		}

		ID3D12CommandQueue* renderQueueHandle()
		{
			return renderQueue.handle();
		}

		ID3D12CommandQueue* computeQueueHandle()
		{
			return computeQueue.handle();
		}

		ID3D12CommandQueue* copyQueueHandle()
		{
			return copyQueue.handle();
		}

	private:
		constexpr static D3D_FEATURE_LEVEL TargetFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		ComUnique<ID3D12Device1> device;
		Queue renderQueue;
		Queue computeQueue;
		Queue copyQueue;

		static ComUnique<ID3D12Device1> makeDevice(vt::Driver& driver, vt::Adapter& adapter)
		{
			auto d3d12CreateDevice = driver.d3d12.deviceCreationFunction();
			ComUnique<ID3D12Device1> device;
			auto result = d3d12CreateDevice(adapter.d3d12.handle(), TargetFeatureLevel, IID_PPV_ARGS(&device));
			vtEnsureResult(result, "Failed to create D3D12 device.");
			return device;
		}
	};
}
