module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <span>
export module Vitro.D3D12.Queue;

import Vitro.Core.Algorithm;
import Vitro.Core.Unique;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Handle;

namespace vt::d3d12
{
	export class Queue
	{
	public:
		Queue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType) :
			queue(makeQueue(device, commandType)), fence(makeFence(device)), fenceEvent(makeEvent())
		{}

		void submit(std::span<CommandListHandle> commandLists)
		{
			auto data = reinterpret_cast<ID3D12CommandList* const*>(commandLists.data());
			queue->ExecuteCommandLists(count(commandLists), data);
		}

		uint64_t signal()
		{
			uint64_t valueToAwait = ++fenceValue;

			auto result = queue->Signal(fence.get(), valueToAwait);
			vtAssertResult(result, "Failed to signal graphics queue.");

			return valueToAwait;
		}

		bool isFenceComplete(uint64_t value) const
		{
			return fence->GetCompletedValue() >= value;
		}

		void awaitFenceValue(uint64_t valueToAwait)
		{
			if(isFenceComplete(valueToAwait))
				return;

			auto result = fence->SetEventOnCompletion(valueToAwait, fenceEvent.get());
			vtAssertResult(result, "Failed to set event on graphics fence completion.");
			::WaitForSingleObject(fenceEvent.get(), INFINITE);
		}

		void flush()
		{
			awaitFenceValue(signal());
		}

		ID3D12CommandQueue* handle()
		{
			return queue.get();
		}

	private:
		uint64_t							fenceValue = 0;
		UniqueInterface<ID3D12CommandQueue> queue;
		UniqueInterface<ID3D12Fence>		fence;
		Unique<HANDLE, ::CloseHandle>		fenceEvent;

		static ID3D12CommandQueue* makeQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType)
		{
			D3D12_COMMAND_QUEUE_DESC const desc {
				.Type = commandType,
			};
			ID3D12CommandQueue* queue;

			auto result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&queue));
			vtEnsureResult(result, "Failed to create D3D12 command queue.");

			return queue;
		}

		ID3D12Fence* makeFence(ID3D12Device* device)
		{
			ID3D12Fence* queueFence;

			auto result = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&queueFence));
			vtEnsureResult(result, "Failed to create D3D12 fence.");

			return queueFence;
		}

		static HANDLE makeEvent()
		{
			auto event = ::CreateEvent(nullptr, false, false, nullptr);
			vtEnsure(event, "Failed to create Windows event.");
			return event;
		}
	};
}
