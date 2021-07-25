module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <span>
export module Vitro.D3D12.Device;

import Vitro.Core.Unique;
import Vitro.D3D12.Queue;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.Driver;
import Vitro.Graphics.Handle;
import Vitro.Graphics.Pipeline;
import Vitro.D3D12.Queue;

namespace vt::d3d12
{
	export class Device final : public DeviceBase
	{
	public:
		Device(vt::Driver& driver, vt::Adapter adapter) :
			device(makeDevice(driver, adapter)),
			renderQueue(device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
			computeQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copyQueue(device.get(), D3D12_COMMAND_LIST_TYPE_COPY)
		{}

		Pipeline makePipeline(PipelineInfo const& info) override
		{
			D3D12_GRAPHICS_PIPELINE_STATE_DESC const desc {
				.pRootSignature = info.rootSignature.d3d12.handle,
				.VS {
					.pShaderBytecode = info.vertexShaderBytecode.data(),
					.BytecodeLength	 = info.vertexShaderBytecode.size(),
				},
				.PS {
					.pShaderBytecode = info.fragmentShaderBytecode.data(),
					.BytecodeLength	 = info.fragmentShaderBytecode.size(),
				},
			};
			ID3D12PipelineState* pipeline;
			auto result = device->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pipeline));
			vtAssertResult(result, "Failed to create render pipeline.");
			return {DeferredUnique<PipelineHandle>({{pipeline}})};
		}

		void submitCopyCommands(std::span<CommandListHandle const> commandLists) override
		{
#if VT_DEBUG
			for(auto list : commandLists)
			{
				bool isRightType = list.d3d12.handle->GetType() == D3D12_COMMAND_LIST_TYPE_COPY;
				vtAssert(isRightType, "All command lists for this submission must be copy command lists.");
			}
#endif
			copyQueue.submit(commandLists);
		}

		void submitComputeCommands(std::span<CommandListHandle const> commandLists) override
		{
#if VT_DEBUG
			for(auto list : commandLists)
			{
				bool isRightType = list.d3d12.handle->GetType() == D3D12_COMMAND_LIST_TYPE_COMPUTE;
				vtAssert(isRightType, "All command lists for this submission must be compute command lists.");
			}
#endif
			computeQueue.submit(commandLists);
		}

		void submitRenderCommands(std::span<CommandListHandle const> commandLists) override
		{
#if VT_DEBUG
			for(auto list : commandLists)
			{
				bool isRightType = list.d3d12.handle->GetType() == D3D12_COMMAND_LIST_TYPE_DIRECT;
				vtAssert(isRightType, "All command lists for this submission must be render command lists.");
			}
#endif
			renderQueue.submit(commandLists);
		}

		ID3D12Device1* handle()
		{
			return device.get();
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
		ComUnique<ID3D12Device1> device;
		Queue renderQueue;
		Queue computeQueue;
		Queue copyQueue;

		static ID3D12Device1* makeDevice(vt::Driver& driver, vt::Adapter& adapter)
		{
			auto d3d12CreateDevice = driver.d3d12.deviceCreationFunction();
			ID3D12Device1* device;
			auto result = d3d12CreateDevice(adapter.d3d12.handle(), Driver::FeatureLevel, IID_PPV_ARGS(&device));
			vtEnsureResult(result, "Failed to create D3D12 device.");
			return device;
		}
	};
}
