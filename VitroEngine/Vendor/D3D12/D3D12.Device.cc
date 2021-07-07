module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.Device;

import Vitro.Core.Unique;
import Vitro.D3D12.ComUnique;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.Driver;
import Vitro.Graphics.Handle;

namespace vt::d3d12
{
	export class Device final : public DeviceBase
	{
	public:
		Device(vt::Driver const& driver, vt::Adapter const& adapter) :
			device(makeDevice(driver, adapter)),
			graphicsQueue(makeQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)),
			computeQueue(makeQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)),
			copyQueue(makeQueue(D3D12_COMMAND_LIST_TYPE_COPY)),
			graphicsFence(makeFence()),
			computeFence(makeFence()),
			copyFence(makeFence()),
			graphicsFenceEvent(makeFenceEvent()),
			computeFenceEvent(makeFenceEvent()),
			copyFenceEvent(makeFenceEvent())
		{}

		void destroyBuffer(BufferHandle const buffer) const override
		{
			buffer.d3d12.handle->Release();
		}

		void destroyPipeline(PipelineHandle const pipeline) const override
		{
			pipeline.d3d12.handle->Release();
		}

		void destroyTexture(TextureHandle const texture) const override
		{
			texture.d3d12.handle->Release();
		}

		ID3D12Device* getDevice() const
		{
			return device;
		}

		ID3D12CommandQueue* getGraphicsQueue() const
		{
			return graphicsQueue;
		}

	private:
		constexpr static D3D_FEATURE_LEVEL TargetFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		ComUnique<ID3D12Device> device;
		ComUnique<ID3D12CommandQueue> graphicsQueue;
		ComUnique<ID3D12CommandQueue> computeQueue;
		ComUnique<ID3D12CommandQueue> copyQueue;
		ComUnique<ID3D12Fence> graphicsFence;
		ComUnique<ID3D12Fence> computeFence;
		ComUnique<ID3D12Fence> copyFence;
		Unique<HANDLE, ::CloseHandle> graphicsFenceEvent;
		Unique<HANDLE, ::CloseHandle> computeFenceEvent;
		Unique<HANDLE, ::CloseHandle> copyFenceEvent;

		static ComUnique<ID3D12Device> makeDevice(vt::Driver const& driver, vt::Adapter const& adapter)
		{
			auto const d3d12CreateDevice = driver.d3d12.getDeviceCreationFunction();
			ComUnique<ID3D12Device> device;
			auto result = d3d12CreateDevice(adapter.d3d12.getHandle(), TargetFeatureLevel, IID_PPV_ARGS(&device));
			vtEnsureResult(result, "Failed to create D3D12 device.");
			return device;
		}

		ComUnique<ID3D12CommandQueue> makeQueue(D3D12_COMMAND_LIST_TYPE const queueType)
		{
			D3D12_COMMAND_QUEUE_DESC const desc {
				.Type = queueType,
			};
			ComUnique<ID3D12CommandQueue> queue;
			auto result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue));
			vtEnsureResult(result, "Failed to create D3D12 command queue.");
			return queue;
		}

		ComUnique<ID3D12Fence> makeFence()
		{
			ComUnique<ID3D12Fence> fence;
			auto result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
			vtEnsureResult(result, "Failed to create D3D12 fence.");
			return fence;
		}

		static Unique<HANDLE, ::CloseHandle> makeFenceEvent()
		{
			Unique<HANDLE, ::CloseHandle> event(::CreateEvent(nullptr, false, false, nullptr));
			vtEnsure(event, "Failed to create Windows event.");
			return event;
		}
	};
}
