module;
#include <vector>
export module vt.Graphics.DeviceBase;

import vt.Core.Array;
import vt.Graphics.AssetResource;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSet;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Handle;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderPassSpecification;
import vt.Graphics.RenderTarget;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.Sampler;
import vt.Graphics.SwapChain;

namespace vt
{
	export class DeviceBase
	{
	public:
		virtual ~DeviceBase() = default;

		virtual std::vector<ComputePipeline> make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs)	  = 0;
		virtual std::vector<RenderPipeline>	 make_render_pipelines(ArrayView<RenderPipelineSpecification> specs)	  = 0;
		virtual std::vector<DescriptorSet>	 make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts)		  = 0;
		virtual DescriptorSetLayout			 make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec) = 0;
		virtual RenderPass					 make_render_pass(RenderPassSpecification const& spec)					  = 0;
		virtual RenderTarget				 make_render_target(RenderTargetSpecification const& spec)				  = 0;
		virtual RootSignature				 make_root_signature(RootSignatureSpecification const& spec)			  = 0;
		virtual Sampler						 make_sampler(SamplerSpecification const& spec)							  = 0;

		virtual void	  recreate_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec)	  = 0;
		virtual SyncValue submit_render_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {})  = 0;
		virtual SyncValue submit_compute_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {}) = 0;
		virtual SyncValue submit_copy_commands(ArrayView<CommandListHandle> cmds, ConstSpan<SyncValue> gpu_syncs = {})	  = 0;

		virtual void wait_for_workload(SyncValue cpu_sync) = 0;
		virtual void flush_render_queue()				   = 0;
		virtual void flush_compute_queue()				   = 0;
		virtual void flush_copy_queue()					   = 0;
		virtual void wait_for_idle()					   = 0;

		virtual SwapChain make_swap_chain(Driver& driver,
										  Window& window,
										  uint8_t buffer_count = SwapChain::DEFAULT_BUFFERS) = 0;

		virtual RenderTarget make_render_target(SharedRenderTargetSpecification const& spec,
												SwapChain const&					   swap_chain,
												unsigned							   back_buffer_index) = 0;

		virtual void recreate_render_target(RenderTarget&						   render_target,
											SharedRenderTargetSpecification const& spec,
											SwapChain const&					   swap_chain,
											unsigned							   back_buffer_index) = 0;

	protected:
		void update_render_target_size(RenderTarget& render_target, unsigned width, unsigned height) const
		{
			render_target.set_width(width);
			render_target.set_height(height);
		}
	};
}
