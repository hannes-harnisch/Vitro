module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.Queue;

import Vitro.Core.Unique;
import Vitro.D3D12.ComUnique;

namespace vt::d3d12
{
	export class Queue
	{
	public:
		Queue(ID3D12Device* const device, D3D12_COMMAND_LIST_TYPE const queueType) :
			queue(makeQueue(device, queueType)), fence(makeFence(device)), fenceEvent(makeEvent())
		{}

		uint64_t signal(uint64_t& fenceValue)
		{
			uint64_t valueToAwait = ++fenceValue;

			auto result = queue->Signal(fence, valueToAwait);
			vtAssertResult(result, "Failed to signal graphics queue.");

			return valueToAwait;
		}

		void awaitFenceValue(uint64_t fenceValue)
		{
			if(fence->GetCompletedValue() < fenceValue)
			{
				auto result = fence->SetEventOnCompletion(fenceValue, fenceEvent);
				vtAssertResult(result, "Failed to set event on graphics fence completion.");
				::WaitForSingleObject(fenceEvent, INFINITE);
			}
		}

		void flush(uint64_t& fenceValue)
		{
			awaitFenceValue(signal(fenceValue));
		}

		ID3D12CommandQueue* getHandle()
		{
			return queue;
		}

	private:
		ComUnique<ID3D12CommandQueue> queue;
		ComUnique<ID3D12Fence> fence;
		Unique<HANDLE, ::CloseHandle> fenceEvent;

		static ComUnique<ID3D12CommandQueue> makeQueue(ID3D12Device* const device, D3D12_COMMAND_LIST_TYPE const queueType)
		{
			D3D12_COMMAND_QUEUE_DESC const desc {
				.Type = queueType,
			};
			ComUnique<ID3D12CommandQueue> queue;
			auto result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue));
			vtEnsureResult(result, "Failed to create D3D12 command queue.");
			return queue;
		}

		static ComUnique<ID3D12Fence> makeFence(ID3D12Device* const device)
		{
			ComUnique<ID3D12Fence> fence;
			auto result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
			vtEnsureResult(result, "Failed to create D3D12 fence.");
			return fence;
		}

		static Unique<HANDLE, ::CloseHandle> makeEvent()
		{
			Unique<HANDLE, ::CloseHandle> event(::CreateEvent(nullptr, false, false, nullptr));
			vtEnsure(event, "Failed to create Windows event.");
			return event;
		}
	};
}
