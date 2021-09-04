module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <span>
export module vt.D3D12.Queue;

import vt.Core.Algorithm;
import vt.D3D12.Utils;
import vt.Graphics.Driver;

namespace vt::d3d12
{
	export class Queue
	{
	public:
		Queue(ID3D12Device4* device, D3D12_COMMAND_LIST_TYPE command_type)
		{
			D3D12_COMMAND_QUEUE_DESC const desc {
				.Type	  = command_type,
				.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
				.Flags	  = D3D12_COMMAND_QUEUE_FLAG_NONE,
				.NodeMask = 0,
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
			wait_for_fence_value(signal());
		}

		uint64_t submit(std::span<vt::CommandListHandle> command_lists)
		{
			auto lists = reinterpret_cast<ID3D12CommandList* const*>(command_lists.data());
			queue->ExecuteCommandLists(count(command_lists), lists);
			return signal();
		}

		void wait_for_fence_value(uint64_t value_to_await) const
		{
			if(fence->GetCompletedValue() >= value_to_await)
				return;

			auto result = fence->SetEventOnCompletion(value_to_await, nullptr);
			VT_ASSERT_RESULT(result, "Failed to wait for queue workload completion.");
		}

		ID3D12CommandQueue* get() const
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
