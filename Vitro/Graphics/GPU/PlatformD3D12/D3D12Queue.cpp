module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Queue;

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
			ID3D12CommandQueue* unowned_queue;

			auto result = device->CreateCommandQueue(&desc, IID_PPV_ARGS(&unowned_queue));
			queue.reset(unowned_queue);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 command queue.");

			ID3D12Fence* unowned_fence;
			result = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&unowned_fence));
			fence.reset(unowned_fence);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 fence.");
		}

		void wait_for_idle()
		{
			wait_for_fence_value(fence.get(), signal());
		}

		void submit(ArrayView<CommandListHandle> cmd_lists, ConstSpan<SyncValue> gpu_syncs)
		{
			for(auto sync : gpu_syncs)
			{
				auto result = queue->Wait(sync.d3d12.wait_fence, sync.d3d12.wait_fence_value);
				VT_ASSERT_RESULT(result, "Failed to insert queue wait for workload.");
			}

			auto lists = reinterpret_cast<ID3D12CommandList* const*>(cmd_lists.data());
			queue->ExecuteCommandLists(count(cmd_lists), lists);
		}

		uint64_t signal()
		{
			auto result = queue->Signal(fence.get(), ++fence_value);
			VT_ASSERT_RESULT(result, "Failed to signal command queue.");

			return fence_value;
		}

		ID3D12Fence* get_fence()
		{
			return fence.get();
		}

		ID3D12CommandQueue* ptr()
		{
			return queue.get();
		}

	private:
		ComUnique<ID3D12CommandQueue> queue;
		ComUnique<ID3D12Fence>		  fence;
		uint64_t					  fence_value = 0;
	};
}
