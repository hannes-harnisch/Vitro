module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <array>
#include <ranges>
#include <span>
#include <vector>
export module vt.Graphics.D3D12.Device;

import vt.Core.Algorithm;
import vt.Core.Array;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.DescriptorSetLayout;
import vt.Graphics.D3D12.RootSignature;
import vt.Graphics.D3D12.SwapChain;
import vt.Graphics.D3D12.Driver;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Queue;
import vt.Graphics.DeviceBase;
import vt.Graphics.Handle;

namespace vt::d3d12
{
	export class D3D12Device final : public DeviceBase
	{
	public:
		D3D12Device(Adapter adapter) :
			device(make_device(std::move(adapter))),
			render_queue(device.get(), D3D12_COMMAND_LIST_TYPE_DIRECT),
			compute_queue(device.get(), D3D12_COMMAND_LIST_TYPE_COMPUTE),
			copy_queue(device.get(), D3D12_COMMAND_LIST_TYPE_COPY),
			descriptor_pool(device.get())
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

		std::vector<ComputePipeline> make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs) override
		{
			std::vector<ComputePipeline> pipelines;
			for(auto& spec : specs)
				pipelines.emplace_back(device.get(), spec);
			return pipelines;
		}

		std::vector<RenderPipeline> make_render_pipelines(ArrayView<RenderPipelineSpecification> specs) override
		{
			std::vector<RenderPipeline> pipelines;
			for(auto& spec : specs)
				pipelines.emplace_back(device.get(), spec);
			return pipelines;
		}

		std::vector<DescriptorSet> make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts) override
		{
			return {};
		}

		DescriptorSetLayout make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec) override
		{
			return {spec};
		}

		RenderPass make_render_pass(RenderPassSpecification const& spec) override
		{
			return {spec};
		}

		RenderTarget make_render_target(RenderTargetSpecification const& spec) override
		{
			RenderTarget::validate_spec(spec);
			return {descriptor_pool, spec};
		}

		RootSignature make_root_signature(RootSignatureSpecification const& spec) override
		{
			return {device.get(), spec};
		}

		Sampler make_sampler(SamplerSpecification const& spec) override
		{
			return {spec};
		}

		SwapChain make_swap_chain(Driver& driver, Window& window, uint8_t buffer_count = SwapChain::DEFAULT_BUFFERS) override
		{
			return {render_queue, driver, window, buffer_count};
		}

		void recreate_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec) override
		{
			render_target.d3d12.recreate(spec);
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

		std::array<ID3D12DescriptorHeap*, 2> get_shader_visible_descriptor_heaps()
		{
			return descriptor_pool.get_shader_visible_heaps();
		}

		ID3D12Device4* ptr()
		{
			return device.get();
		}

	private:
		ComUnique<ID3D12Device4> device;
		Queue					 render_queue;
		Queue					 compute_queue;
		Queue					 copy_queue;
		DescriptorPool			 descriptor_pool;

		static ComUnique<ID3D12Device4> make_device(Adapter adapter)
		{
			ID3D12Device4* raw_device;

			auto result = D3D12CreateDevice(adapter.d3d12.get(), D3D12Driver::MIN_FEATURE_LEVEL, IID_PPV_ARGS(&raw_device));

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
