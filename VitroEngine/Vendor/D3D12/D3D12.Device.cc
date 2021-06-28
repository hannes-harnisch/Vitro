module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.Device;

import Vitro.D3D12.Unique;
import Vitro.Graphics.Adapter;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.Driver;
import Vitro.Graphics.Handle;

namespace D3D12
{
	export class Device final : public DeviceBase
	{
	public:
		Device(::Adapter const& adapter, ::Driver const& driver) :
			device(makeDevice(adapter, driver)),
			graphicsQueue(makeQueue(D3D12_COMMAND_LIST_TYPE_DIRECT)),
			computeQueue(makeQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE)),
			copyQueue(makeQueue(D3D12_COMMAND_LIST_TYPE_COPY))
		{}

		void destroyBuffer(BufferHandle const buffer) override
		{
			buffer.d3d12.handle->Release();
		}

		void destroyPipeline(PipelineHandle const pipeline) override
		{
			pipeline.d3d12.handle->Release();
		}

		void destroyTexture(TextureHandle const texture) override
		{
			texture.d3d12.handle->Release();
		}

		ID3D12CommandQueue* getGraphicsQueue() const
		{
			return graphicsQueue;
		}

	private:
		constexpr static auto TargetFeatureLevel = D3D_FEATURE_LEVEL_11_0;

		Unique<ID3D12Device> device;
		Unique<ID3D12CommandQueue> graphicsQueue;
		Unique<ID3D12CommandQueue> computeQueue;
		Unique<ID3D12CommandQueue> copyQueue;

		static Unique<ID3D12Device> makeDevice(::Adapter const& adapter, ::Driver const& driver)
		{
			auto d3d12CreateDevice = driver.d3d12().getDeviceCreationFunction();

			Unique<ID3D12Device> device;
			auto result = d3d12CreateDevice(adapter.d3d12().handle(), TargetFeatureLevel, IID_PPV_ARGS(&device));
			vtEnsureResult(result, "Failed to create D3D12 device.");

			return device;
		}

		Unique<ID3D12CommandQueue> makeQueue(D3D12_COMMAND_LIST_TYPE const queueType)
		{
			D3D12_COMMAND_QUEUE_DESC desc {.Type = queueType};

			Unique<ID3D12CommandQueue> queue;
			auto result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue));
			vtEnsureResult(result, "Failed to create D3D12 command queue.");

			return queue;
		}
	};
}
