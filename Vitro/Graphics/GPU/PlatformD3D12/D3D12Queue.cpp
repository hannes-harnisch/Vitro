module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Queue;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.Handle;

namespace vt::d3d12
{
	export void wait_for_fence_value(ID3D12Fence* fence, uint64_t value_to_await)
	{
		if(!fence || fence->GetCompletedValue() >= value_to_await)
			return;

		auto result = fence->SetEventOnCompletion(value_to_await, nullptr);
		VT_ASSERT_RESULT(result, "Failed to wait for queue workload completion.");
	}

	export class Queue
	{
	public:
		Queue(ID3D12Device4* device, D3D12_COMMAND_LIST_TYPE command_type)
		{
			D3D12_COMMAND_QUEUE_DESC const desc {
				.Type	  = command_type,
				.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
				.Flags	  = D3D12_COMMAND_QUEUE_FLAG_NONE,
			};
			ID3D12CommandQueue* raw_queue;

			auto result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&raw_queue));
			queue.reset(raw_queue);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 command queue.");

			ID3D12Fence* raw_fence;
			result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&raw_fence));
			fence.reset(raw_fence);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 fence.");
		}

		void wait_for_idle()
		{
			wait_for_fence_value(fence.get(), signal());
		}

		D3D12SyncValue submit(ArrayView<CommandListHandle> cmd_lists, ConstSpan<SyncValue> gpu_syncs)
		{
			for(auto sync : gpu_syncs)
			{
				auto result = queue->Wait(sync.d3d12.wait_fence, sync.d3d12.wait_fence_value);
				VT_ASSERT_RESULT(result, "Failed to insert queue wait for workload.");
			}

			auto lists = reinterpret_cast<ID3D12CommandList* const*>(cmd_lists.data());
			queue->ExecuteCommandLists(count(cmd_lists), lists);
			return {
				.wait_fence		  = fence.get(),
				.wait_fence_value = signal(),
			};
		}

		ID3D12CommandQueue* ptr() const // TODO: no const here
		{
			return queue.get();
		}

	private:
		ComUnique<ID3D12CommandQueue> queue;
		ComUnique<ID3D12Fence>		  fence;
		uint64_t					  fence_value = 0;

		uint64_t signal()
		{
			auto result = queue->Signal(fence.get(), ++fence_value);
			VT_ASSERT_RESULT(result, "Failed to signal command queue.");

			return fence_value;
		}
	};
}
