module;
#include <vector>
export module vt.Graphics.DeviceBase;

import vt.App.Window;
import vt.Core.Array;
import vt.Core.Ref;
import vt.Core.SmallList;
import vt.Core.Specification;
import vt.Graphics.AssetResource;
import vt.Graphics.CommandList;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSet;
import vt.Graphics.DescriptorSetLayout;
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
	// Describes an update to a single binding of one descriptor set. Multiple descriptors in the same binding can be update
	export struct DescriptorUpdate
	{
		DescriptorSet&	   set;
		Explicit<unsigned> binding;
		unsigned		   start_array_index = 0;
		union
		{
			ConstSpan<CRef<Image>>	 images;
			ConstSpan<CRef<Sampler>> samplers;
			ConstSpan<CRef<Buffer>>	 buffers;
		};
	};

	export class DeviceBase
	{
	public:
		virtual ~DeviceBase() = default;

		// Makes a command list for copy and transfer operations.
		virtual CopyCommandList make_copy_command_list() = 0;

		// Makes a command list for compute operations.
		virtual ComputeCommandList make_compute_command_list() = 0;

		// Makes a command list for rendering operations.
		virtual RenderCommandList make_render_command_list() = 0;

		// Makes a buffer from a specification.
		virtual Buffer make_buffer(BufferSpecification const& spec) = 0;

		// Makes an image from a specification.
		virtual Image make_image(ImageSpecification const& spec) = 0;

		// Makes pipeline objects usable in compute commands. It can be more efficient to create many at once.
		virtual std::vector<ComputePipeline> make_compute_pipelines(ArrayView<ComputePipelineSpecification> specs) = 0;

		// Makes pipeline objects usable in rendering commands. It can be more efficient to create many at once.
		virtual std::vector<RenderPipeline> make_render_pipelines(ArrayView<RenderPipelineSpecification> specs) = 0;

		// Makes descriptor sets that do not yet represent any resources (empty descriptor sets). It can be more efficient to
		// create many at once.
		virtual SmallList<DescriptorSet> make_descriptor_sets(ArrayView<DescriptorSetLayout> set_layouts) = 0;

		// Makes a descriptor set layout from a specification.
		virtual DescriptorSetLayout make_descriptor_set_layout(DescriptorSetLayoutSpecification const& spec) = 0;

		// Makes a render pass from a specification.
		virtual RenderPass make_render_pass(RenderPassSpecification const& spec) = 0;

		// Makes a root signature from a specification.
		virtual RootSignature make_root_signature(RootSignatureSpecification const& spec) = 0;

		// Makes a sampler from a specification.
		virtual Sampler make_sampler(SamplerSpecification const& spec) = 0;

		// Makes a shader from a bytecode file located at the given path.
		virtual Shader make_shader(char const path[]) = 0;

		// Makes a swap chain associated with the given window.
		virtual SwapChain make_swap_chain(Window& window, uint8_t buffer_count = SwapChain::DEFAULT_BUFFERS) = 0;

		// Update descriptor sets, associating them with new resources.
		virtual void update_descriptors(ArrayView<DescriptorUpdate> updates) = 0;

		// Returns a writable pointer to the GPU memory backing the given buffer.
		virtual void* map(Buffer const& buffer) = 0;

		// Returns a writable pointer to the GPU memory backing the given image.
		virtual void* map(Image const& image) = 0;

		// Invalidates the pointer previously returned from a map call.
		virtual void unmap(Buffer const& buffer) = 0;

		// Invalidates the pointer previously returned from a map call.
		virtual void unmap(Image const& image) = 0;

		// Submits render command lists to the GPU. The returned token can be used to issue waits on the submitted workload.
		virtual SyncToken submit_render_commands(ArrayView<CommandListHandle> cmds,
												 ConstSpan<SyncToken>		  gpu_wait_tokens = {}) = 0;

		// Submits compute command lists to the GPU. The returned token can be used to issue waits on the submitted workload.
		virtual SyncToken submit_compute_commands(ArrayView<CommandListHandle> cmds,
												  ConstSpan<SyncToken>		   gpu_wait_tokens = {}) = 0;

		// Submits copy command lists to the GPU. The returned token can be used to issue waits on the submitted workload.
		virtual SyncToken submit_copy_commands(ArrayView<CommandListHandle> cmds,
											   ConstSpan<SyncToken>			gpu_wait_tokens = {}) = 0;

		// Submits render command lists to the GPU and presents the next swap chain image. The returned token can be used to
		// issue waits on the submitted workload, but not the presentation.
		virtual SyncToken submit_for_present(ArrayView<CommandListHandle> cmds,
											 SwapChain&					  swap_chain,
											 ConstSpan<SyncToken>		  gpu_wait_tokens = {}) = 0;

		// Makes the CPU wait until the workload associated with the token is finished.
		virtual void wait_for_workload(SyncToken cpu_wait_token) = 0;

		// Makes the CPU wait until all currently submitted rendering workloads are finished on the GPU.
		virtual void flush_render_queue() = 0;

		// Makes the CPU wait until all currently submitted compute workloads are finished on the GPU.
		virtual void flush_compute_queue() = 0;

		// Makes the CPU wait until all currently submitted copy workloads are finished on the GPU.
		virtual void flush_copy_queue() = 0;

		// Makes the CPU wait until all currently submitted workloads are finished on the GPU.
		virtual void flush() = 0;

		// Makes a render target from a specification.
		RenderTarget make_render_target(RenderTargetSpecification const& spec)
		{
			validate_render_target_spec(spec);
			return make_platform_render_target(spec);
		}

		// Makes a render target from a specification aimed to be reused between renderers.
		RenderTarget make_render_target(SharedRenderTargetSpecification const& spec,
										SwapChain const&					   swap_chain,
										unsigned							   back_buffer_index)
		{
			validate_shared_target_spec(spec, swap_chain, back_buffer_index);
			return make_platform_render_target(spec, swap_chain, back_buffer_index);
		}

		// Recreates a render target after it was invalidated, such as by resizing a window.
		void recreate_render_target(RenderTarget& render_target, RenderTargetSpecification const& spec)
		{
			recreate_platform_render_target(render_target, spec);
			update_render_target_size(render_target, spec.width, spec.height);
		}

		// Recreates a shared render target after it was invalidated, such as by resizing a window.
		void recreate_render_target(RenderTarget&						   render_target,
									SharedRenderTargetSpecification const& spec,
									SwapChain const&					   swap_chain,
									unsigned							   back_buffer_index)
		{
			recreate_platform_render_target(render_target, spec, swap_chain, back_buffer_index);
			auto [width, height] = swap_chain->get_size();
			update_render_target_size(render_target, width, height);
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
