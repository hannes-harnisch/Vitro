module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <ranges>
#include <span>
export module vt.D3D12.Device;

import vt.D3D12.Queue;
import vt.D3D12.Utils;
import vt.Graphics.Adapter;
import vt.Graphics.DeviceBase;
import vt.Graphics.Driver;

namespace vt::d3d12
{
	export class Device final : public DeviceBase
	{
	public:
		PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER const d3d12_create_versioned_root_signature_deserializer;
		PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE const			 d3d12_serialize_versioned_root_signature;

		Device(vt::Driver const& driver, vt::Adapter adapter) :
			d3d12_create_versioned_root_signature_deserializer(
				driver.d3d12.load_d3d12_function<PFN_D3D12_CREATE_VERSIONED_ROOT_SIGNATURE_DESERIALIZER>(
					"D3D12CreateVersionedRootSignatureDeserializer")),
			d3d12_serialize_versioned_root_signature(
				driver.d3d12.load_d3d12_function<PFN_D3D12_SERIALIZE_VERSIONED_ROOT_SIGNATURE>(
					"D3D12SerializeVersionedRootSignature")),
			device(make_device(driver, std::move(adapter))),
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
					  "graphics drivers. If the problem persists, you may need to update your graphics card.");
		}

		Receipt submit_render_commands(std::span<vt::CommandListHandle> command_lists) override
		{
#if VT_DEBUG
			validate_command_lists(command_lists, D3D12_COMMAND_LIST_TYPE_DIRECT);
#endif
			return {render_queue.submit(command_lists)};
		}

		Receipt submit_compute_commands(std::span<vt::CommandListHandle> command_lists) override
		{
#if VT_DEBUG
			validate_command_lists(command_lists, D3D12_COMMAND_LIST_TYPE_COMPUTE);
#endif
			return {compute_queue.submit(command_lists)};
		}

		Receipt submit_copy_commands(std::span<vt::CommandListHandle> command_lists) override
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

		ID3D12Device4* get() const
		{
			return device.get();
		}

		ID3D12CommandQueue* render_queue_handle() const
		{
			return render_queue.get();
		}

	private:
		ComUnique<ID3D12Device4> device;
		Queue					 render_queue;
		Queue					 compute_queue;
		Queue					 copy_queue;

		static decltype(device) make_device(vt::Driver const& driver, vt::Adapter adapter)
		{
			ID3D12Device4* raw_device;

			auto result = driver.d3d12.d3d12_create_device(adapter.d3d12.get(), Driver::FeatureLevel,
														   IID_PPV_ARGS(&raw_device));

			decltype(device) fresh_device(raw_device);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 device.");

			return fresh_device;
		}

		static void validate_command_lists(std::span<vt::CommandListHandle> command_lists, D3D12_COMMAND_LIST_TYPE type)
		{
			for(auto list : command_lists)
			{
				bool is_right_type = list.d3d12->GetType() == type;
				VT_ASSERT(is_right_type, "The type of this command list does not match the type of queue it was submitted to.");
			}
		}
	};
}
