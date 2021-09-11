module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <ranges>
#include <span>
export module vt.D3D12.Device;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.D3D12.Utils;
import vt.Graphics.Adapter;
import vt.Graphics.DeviceBase;
import vt.Graphics.Driver;

namespace vt::d3d12
{
	class Queue
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

		uint64_t submit(CSpan<CommandListHandle> command_lists)
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
		D3D12Device(Driver const&, Adapter adapter) :
			device(make_device(std::move(adapter))),
			render_queue(device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
			compute_queue(device.get(), D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copy_queue(device.get(), D3D12_COMMAND_LIST_TYPE_COPY)
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

		Receipt submit_render_commands(CSpan<CommandListHandle> command_lists) override
		{
#if VT_DEBUG
			validate_command_lists(command_lists, D3D12_COMMAND_LIST_TYPE_DIRECT);
#endif
			return {render_queue.submit(command_lists)};
		}

		Receipt submit_compute_commands(CSpan<CommandListHandle> command_lists) override
		{
#if VT_DEBUG
			validate_command_lists(command_lists, D3D12_COMMAND_LIST_TYPE_COMPUTE);
#endif
			return {compute_queue.submit(command_lists)};
		}

		Receipt submit_copy_commands(CSpan<CommandListHandle> command_lists) override
		{
#if VT_DEBUG
			validate_command_lists(command_lists, D3D12_COMMAND_LIST_TYPE_COPY);
#endif
			return {copy_queue.submit(command_lists)};
		}

		void wait_for_render_workload(Receipt receipt) override
		{
			render_queue.wait_for_fence_value(receipt.host_wait_value);
		}

		void wait_for_compute_workload(Receipt receipt) override
		{
			compute_queue.wait_for_fence_value(receipt.host_wait_value);
		}

		void wait_for_copy_workload(Receipt receipt) override
		{
			copy_queue.wait_for_fence_value(receipt.host_wait_value);
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

		ID3D12Device4* ptr() const
		{
			return device.get();
		}

		ID3D12CommandQueue* get_render_queue_ptr() const
		{
			return render_queue.ptr();
		}

	private:
		ComUnique<ID3D12Device4> device;
		Queue					 render_queue;
		Queue					 compute_queue;
		Queue					 copy_queue;

		static ComUnique<ID3D12Device4> make_device(Adapter adapter)
		{
			ID3D12Device4* raw_device;

			auto result = D3D12CreateDevice(adapter.d3d12.ptr(), D3D12Driver::MinimumFeatureLevel, IID_PPV_ARGS(&raw_device));

			ComUnique<ID3D12Device4> fresh_device(raw_device);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 device.");

			return fresh_device;
		}

		static void validate_command_lists(CSpan<CommandListHandle> command_lists, D3D12_COMMAND_LIST_TYPE type)
		{
			for(auto list : command_lists)
			{
				bool is_right_type = list.d3d12->GetType() == type;
				VT_ASSERT(is_right_type, "The type of this command list does not match the type of queue it was submitted to.");
			}
		}
	};
}
