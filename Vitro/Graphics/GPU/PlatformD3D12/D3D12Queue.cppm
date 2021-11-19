module;
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Queue;

import vt.Core.Array;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.Handle;

namespace vt::d3d12
{
	export void wait_for_fence_value(ID3D12Fence* fence, uint64_t value_to_await);

	export class Queue
	{
	public:
		Queue(ID3D12Device4& device, D3D12_COMMAND_LIST_TYPE command_type);

		void				flush();
		void				submit(ArrayView<CommandListHandle> cmd_lists, ConstSpan<SyncToken> gpu_wait_tokens) const;
		uint64_t			signal();
		ID3D12Fence*		get_fence() const;
		ID3D12CommandQueue* get_handle() const;

	private:
		ComUnique<ID3D12CommandQueue> queue;
		ComUnique<ID3D12Fence>		  fence;
		uint64_t					  fence_value = 0;
	};
}
