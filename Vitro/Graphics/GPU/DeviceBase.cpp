module;
#include <vector>
export module vt.Graphics.DeviceBase;

import vt.App.Window;
import vt.Core.Array;
import vt.Graphics.AssetResource;
import vt.Graphics.CommandList;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSet;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Driver;
import vt.Graphics.Handle;
import vt.Graphics.PipelineSpecification;
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

		virtual CopyCommandList				 make_copy_command_list()												  = 0;
		virtual ComputeCommandList			 make_compute_command_list()											  = 0;
		virtual RenderCommandList			 make_render_command_list()												  = 0;
		virtual std::vector<ComputePipeline> make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs)	  = 0;
		virtual std::vector<RenderPipeline>	 make_render_pipelines(ArrayView<RenderPipelineSpecification> specs)	  = 0;
		virtual std::vector<DescriptorSet>	 make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts)		  = 0;
		virtual DescriptorSetLayout			 make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec) = 0;
		virtual RenderPass					 make_render_pass(RenderPassSpecification const& spec)					  = 0;
		virtual RootSignature				 make_root_signature(RootSignatureSpecification const& spec)			  = 0;
		virtual Sampler						 make_sampler(SamplerSpecification const& spec)							  = 0;
		virtual Shader						 make_shader(char const path[])											  = 0;

		virtual SwapChain make_swap_chain(Driver& driver,
										  Window& window,
										  uint8_t buffer_count = SwapChain::DEFAULT_BUFFERS) = 0;

		virtual SyncToken submit_render_commands(ArrayView<CommandListHandle> cmds,
												 ConstSpan<SyncToken>		  gpu_wait_tokens = {})	 = 0;
		virtual SyncToken submit_compute_commands(ArrayView<CommandListHandle> cmds,
												  ConstSpan<SyncToken>		   gpu_wait_tokens = {}) = 0;
		virtual SyncToken submit_copy_commands(ArrayView<CommandListHandle> cmds,
											   ConstSpan<SyncToken>			gpu_wait_tokens = {})	 = 0;
		virtual SyncToken submit_for_present(ArrayView<CommandListHandle> cmds,
											 SwapChain&					  swap_chain,
											 ConstSpan<SyncToken>		  gpu_wait_tokens = {})		 = 0;

		virtual void wait_for_workload(SyncToken cpu_wait_token) = 0;
		virtual void flush_render_queue()						 = 0;
		virtual void flush_compute_queue()						 = 0;
		virtual void flush_copy_queue()							 = 0;
		virtual void flush()									 = 0;

		RenderTarget make_render_target(RenderTargetSpecification const& spec)
		{
			validate_render_target_spec(spec);
			return make_platform_render_target(spec);
		}

		RenderTarget make_render_target(SharedRenderTargetSpecification const& spec,
										SwapChain const&					   swap_chain,
										unsigned							   back_buffer_index)
		{
			validate_shared_target_spec(spec, swap_chain, back_buffer_index);
			return make_platform_render_target(spec, swap_chain, back_buffer_index);
		}

		void recreate_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec)
		{
			recreate_platform_render_target(render_target, spec);
			update_render_target_size(render_target, spec.width, spec.height);
		}

		void recreate_render_target(RenderTarget&						   render_target,
									SharedRenderTargetSpecification const& spec,
									SwapChain const&					   swap_chain,
									unsigned							   back_buffer_index)
		{
			recreate_platform_render_target(render_target, spec, swap_chain, back_buffer_index);
			update_render_target_size(render_target, swap_chain->get_width(), swap_chain->get_height());
		}

	private:
		virtual RenderTarget make_platform_render_target(RenderTargetSpecification const& spec) = 0;

		virtual RenderTarget make_platform_render_target(SharedRenderTargetSpecification const& spec,
														 SwapChain const&						swap_chain,
														 unsigned								back_buffer_index) = 0;

		virtual void recreate_platform_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec) = 0;

		virtual void recreate_platform_render_target(RenderTarget&							render_target,
													 SharedRenderTargetSpecification const& spec,
													 SwapChain const&						swap_chain,
													 unsigned								back_buffer_index) = 0;

		void update_render_target_size(RenderTarget& render_target, unsigned width, unsigned height) const
		{
			render_target.set_width(width);
			render_target.set_height(height);
		}
	};
}
