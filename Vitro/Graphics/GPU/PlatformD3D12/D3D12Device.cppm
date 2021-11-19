module;
#include "D3D12API.hpp"

#include <memory>
export module vt.Graphics.D3D12.Device;

import vt.Core.Array;
import vt.Core.SmallList;
import vt.Graphics.D3D12.DescriptorPool;
import vt.Graphics.D3D12.DescriptorSetLayout;
import vt.Graphics.D3D12.RootSignature;
import vt.Graphics.D3D12.SwapChain;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Queue;
import vt.Graphics.AbstractDevice;
import vt.Graphics.Handle;
import vt.Graphics.RingBuffer;

namespace vt::d3d12
{
	export class D3D12Device final : public AbstractDevice
	{
	public:
		static constexpr D3D_FEATURE_LEVEL MIN_FEATURE_LEVEL = D3D_FEATURE_LEVEL_11_1;

		D3D12Device(Adapter const& adapter, IDXGIFactory5& factory);
		CopyCommandList				 make_copy_command_list() override;
		ComputeCommandList			 make_compute_command_list() override;
		RenderCommandList			 make_render_command_list() override;
		Buffer						 make_buffer(BufferSpecification const& spec) override;
		Image						 make_image(ImageSpecification const& spec) override;
		std::vector<ComputePipeline> make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs) override;
		std::vector<RenderPipeline>	 make_render_pipelines(ArrayView<RenderPipelineSpecification> specs) override;
		SmallList<DescriptorSet>	 make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts,
														  unsigned const				 dynamic_counts[]) override;
		DescriptorSetLayout			 make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec) override;
		RenderPass					 make_render_pass(RenderPassSpecification const& spec) override;
		RootSignature				 make_root_signature(RootSignatureSpecification const& spec) override;
		Sampler						 make_sampler(SamplerSpecification const& spec) override;
		Shader						 make_shader(char const path[]) override;
		SwapChain make_swap_chain(Window& window, uint8_t buffer_count = SwapChain::DEFAULT_BUFFERS) override;
		void	  update_descriptors(ArrayView<DescriptorUpdate> updates) override;
		void	  reset_descriptors() override;
		void*	  map(Buffer const& buffer) override;
		void*	  map(Image const& image) override;
		void	  unmap(Buffer const& buffer) override;
		void	  unmap(Image const& image) override;
		SyncToken submit_render_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens = {}) override;
		SyncToken submit_compute_commands(ArrayView<CommandListHandle> cmds,
										  ConstSpan<SyncToken>		   gpu_wait_tokens = {}) override;
		SyncToken submit_copy_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncToken> gpu_wait_tokens = {}) override;
		SyncToken submit_for_present(ArrayView<CommandListHandle> cmds,
									 SwapChain&					  swap_chain,
									 ConstSpan<SyncToken>		  gpu_wait_tokens = {}) override;
		void	  wait_for_workload(SyncToken cpu_wait_token) override;
		void	  flush_render_queue() override;
		void	  flush_compute_queue() override;
		void	  flush_copy_queue() override;
		void	  flush() override;
		unsigned  get_max_resource_descriptors_per_stage() const override;
		unsigned  get_max_sampler_descriptors_per_stage() const override;

	private:
		IDXGIFactory5*					  factory;
		ComUnique<ID3D12Device4>		  device;
		D3D12_RESOURCE_BINDING_TIER		  binding_tier;
		Queue							  render_queue;
		Queue							  compute_queue;
		Queue							  copy_queue;
		std::unique_ptr<DescriptorPool>	  descriptor_pool;
		ComUnique<ID3D12CommandSignature> dispatch_signature;
		ComUnique<ID3D12CommandSignature> draw_signature;
		ComUnique<ID3D12CommandSignature> draw_indexed_signature;
		ComUnique<D3D12MA::Allocator>	  allocator;

		D3D12_RESOURCE_BINDING_TIER query_resource_binding_tier() const;
		RenderTarget				make_platform_render_target(RenderTargetSpecification const& spec) override;
		RenderTarget				make_platform_render_target(SharedRenderTargetSpecification const& spec,
																SwapChain const&					   swap_chain,
																unsigned							   back_buffer_index) override;
		void recreate_platform_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec) override;
		void recreate_platform_render_target(RenderTarget&							render_target,
											 SharedRenderTargetSpecification const& spec,
											 SwapChain const&						swap_chain,
											 unsigned								back_buffer_index) override;
		void ensure_required_features_exist() const;
		void initialize_command_signatures();
		void initialize_allocator(Adapter const& adapter);
	};
}
