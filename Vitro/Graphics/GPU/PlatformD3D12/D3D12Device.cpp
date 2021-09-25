module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <ranges>
#include <span>
export module vt.Graphics.D3D12.Device;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Graphics.D3D12.DescriptorAllocator;
import vt.Graphics.D3D12.Driver;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.DeviceBase;
import vt.Graphics.Handle;

namespace vt::d3d12
{
	void wait_for_fence_value(ID3D12Fence* fence, uint64_t value_to_await)
	{
		if(!fence || fence->GetCompletedValue() >= value_to_await)
			return;

		auto result = fence->SetEventOnCompletion(value_to_await, nullptr);
		VT_ASSERT_RESULT(result, "Failed to wait for queue workload completion.");
	}

	class Queue
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

		ID3D12CommandQueue* ptr() const
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

	export class D3D12Device final : public DeviceBase
	{
	public:
		D3D12Device(Adapter adapter) :
			device(make_device(std::move(adapter))),
			render_queue(device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
			compute_queue(device.get(), D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copy_queue(device.get(), D3D12_COMMAND_LIST_TYPE_COPY),
			rtv_allocator(device.get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, MaxRenderTargetViews, false),
			dsv_allocator(device.get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, MaxDepthStencilViews, false)
		{
			D3D12_FEATURE_DATA_ROOT_SIGNATURE feature {
				.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1,
			};
			auto result = device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &feature, sizeof feature);
			VT_ENSURE_RESULT(result, "Failed to check for D3D12 root signature feature support.");
			VT_ENSURE(feature.HighestVersion == D3D_ROOT_SIGNATURE_VERSION_1_1,
					  "The DirectX 12 root signature 1.1 feature isn't available. Try updating your Windows version and your "
					  "graphics drivers. If the problem persists, you may need to switch to a newer GPU.");
		}

		SyncValue submit_render_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) override
		{
			validate_command_lists(cmds, D3D12_COMMAND_LIST_TYPE_DIRECT);
			return {render_queue.submit(cmds, gpu_syncs)};
		}

		SyncValue submit_compute_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) override
		{
			validate_command_lists(cmds, D3D12_COMMAND_LIST_TYPE_COMPUTE);
			return {compute_queue.submit(cmds, gpu_syncs)};
		}

		SyncValue submit_copy_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) override
		{
			validate_command_lists(cmds, D3D12_COMMAND_LIST_TYPE_COPY);
			return {copy_queue.submit(cmds, gpu_syncs)};
		}

		void wait_for_workload(SyncValue cpu_sync) override
		{
			wait_for_fence_value(cpu_sync.d3d12.wait_fence, cpu_sync.d3d12.wait_fence_value);
		}

		void flush_render_queue() override
		{
			render_queue.wait_for_idle();
		}

		void flush_compute_queue() override
		{
			compute_queue.wait_for_idle();
		}

		void flush_copy_queue() override
		{
			copy_queue.wait_for_idle();
		}

		void wait_for_idle() override
		{
			render_queue.wait_for_idle();
			compute_queue.wait_for_idle();
			copy_queue.wait_for_idle();
		}

		DescriptorAllocation allocate_render_target_views(unsigned count)
		{
			return rtv_allocator.allocate(count);
		}

		DescriptorAllocation allocate_depth_stencil_view()
		{
			return dsv_allocator.allocate(1);
		}

		void deallocate_render_target_views(DescriptorAllocation rtvs)
		{
			rtv_allocator.deallocate(rtvs);
		}

		void deallocate_depth_stencil_views(DescriptorAllocation dsvs)
		{
			dsv_allocator.deallocate(dsvs);
		}

		unsigned get_render_target_view_size() const
		{
			return rtv_allocator.get_increment_size();
		}

		unsigned get_depth_stencil_view_size() const
		{
			return dsv_allocator.get_increment_size();
		}

		ID3D12Device4* ptr()
		{
			return device.get();
		}

		ID3D12CommandQueue* get_render_queue_ptr()
		{
			return render_queue.ptr();
		}

	private:
		static constexpr unsigned MaxRenderTargetViews = 120; // Arbitrarily chosen.
		static constexpr unsigned MaxDepthStencilViews = 40;  // Arbitrarily chosen.

		ComUnique<ID3D12Device4> device;
		Queue					 render_queue;
		Queue					 compute_queue;
		Queue					 copy_queue;
		DescriptorAllocator		 rtv_allocator;
		DescriptorAllocator		 dsv_allocator;

		static ComUnique<ID3D12Device4> make_device(Adapter adapter)
		{
			ID3D12Device4* raw_device;

			auto result = D3D12CreateDevice(adapter.d3d12.get(), D3D12Driver::MinimumFeatureLevel, IID_PPV_ARGS(&raw_device));

			ComUnique<ID3D12Device4> fresh_device(raw_device);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 device.");

			return fresh_device;
		}

		static void validate_command_lists([[maybe_unused]] ArrayView<CommandListHandle> cmd_lists,
										   [[maybe_unused]] D3D12_COMMAND_LIST_TYPE		 type)
		{
#if VT_DEBUG
			for(auto list : cmd_lists)
			{
				bool is_right_type = list.d3d12->GetType() == type;
				VT_ASSERT(is_right_type, "The type of this command list does not match the type of queue it was submitted to.");
			}
#endif
		}
	};
}
