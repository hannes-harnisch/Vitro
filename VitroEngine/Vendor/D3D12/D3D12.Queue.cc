module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <span>
export module Vitro.D3D12.Queue;

import Vitro.Core.Algorithm;
import Vitro.Core.Unique;
import Vitro.D3D12.Utils;
import Vitro.Trace.Log;

namespace vt::d3d12
{
	export class Queue
	{
	public:
		Queue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType) :
			queue(makeQueue(device, commandType)), fence(makeFence(device)), event(makeEvent())
		{}

		~Queue()
		{
			if(queue)
				waitForIdle();
		}

		void waitForIdle()
		{
			waitForFenceValue(signal());
		}

		uint64_t submit(std::span<void*> commandLists)
		{
			auto lists = reinterpret_cast<ID3D12CommandList* const*>(commandLists.data());
			queue->ExecuteCommandLists(count(commandLists), lists);
			return signal();
		}

		void waitForFenceValue(uint64_t valueToAwait) const
		{
			if(fence->GetCompletedValue() >= valueToAwait)
				return;

			auto result = fence->SetEventOnCompletion(valueToAwait, event.get());
			vtAssertResult(result, "Failed to set OS event for queue workload completion.");
			::WaitForSingleObject(event.get(), INFINITE);
		}

		ID3D12CommandQueue* get() const
		{
			return queue.get();
		}

	private:
		ComUnique<ID3D12CommandQueue> queue;
		ComUnique<ID3D12Fence>		  fence;
		Unique<HANDLE, ::CloseHandle> event;
		uint64_t					  fenceValue = 0;

		static decltype(queue) makeQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType)
		{
			D3D12_COMMAND_QUEUE_DESC const desc {
				.Type	  = commandType,
				.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
				.Flags	  = D3D12_COMMAND_QUEUE_FLAG_NONE,
				.NodeMask = 0,
			};
			ID3D12CommandQueue* rawQueue;

			auto result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&rawQueue));

			decltype(queue) freshQueue(rawQueue);
			vtEnsureResult(result, "Failed to create D3D12 command queue.");

			return freshQueue;
		}

		static decltype(fence) makeFence(ID3D12Device* device)
		{
			ID3D12Fence* rawFence;

			auto result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&rawFence));

			decltype(fence) freshFence(rawFence);
			vtEnsureResult(result, "Failed to create D3D12 fence.");

			return freshFence;
		}

		static decltype(event) makeEvent()
		{
			auto rawEvent = ::CreateEventW(nullptr, false, false, nullptr);

			decltype(event) freshEvent(rawEvent);
			vtEnsure(rawEvent, "Failed to create Windows event.");

			return freshEvent;
		}

		uint64_t signal()
		{
			auto result = queue->Signal(fence.get(), ++fenceValue);
			vtAssertResult(result, "Failed to signal command queue.");

			return fenceValue;
		}
	};
}
