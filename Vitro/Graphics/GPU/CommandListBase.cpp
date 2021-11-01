module;
#include <span>
export module vt.Graphics.CommandListBase;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.Rect;
import vt.Core.Vector;
import vt.Graphics.AssetResource;
import vt.Graphics.DescriptorSet;
import vt.Graphics.Handle;
import vt.Graphics.PipelineSpecification;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RootSignature;

namespace vt
{
	export enum class CommandType : uint8_t {
		Copy,
		Compute,
		Render,
	};

	// Holds either a clear value for a color attachment or a depth stencil attachment.
	export union ClearValue
	{
		Float4 color = {};
		struct
		{
			float	 depth;
			unsigned stencil;
		};
	};

	// Describes which subresource of an image at which offset to copy from, and to which subresource at what offset to
	// copy it to.
	export struct ImageCopyRegion
	{
		Expanse	 expanse;
		UInt3	 src_offset		 = {};
		UInt3	 dst_offset		 = {};
		unsigned src_mip		 = 0; // Mip level of the source image.
		unsigned src_array_index = 0; // Index of the source image in a texture array.
		unsigned dst_mip		 = 0; // Mip level of the destination image.
		unsigned dst_array_index = 0; // Index of the destination image in a texture array.
	};

	export class CopyCommandListBase
	{
	public:
		virtual ~CopyCommandListBase() = default;

		// Gets a handle to the command list that can be used to easily group command lists for submission.
		virtual CommandListHandle get_handle() = 0;

		// Resets the command list. To be called at the start of a frame.
		virtual void reset() = 0;

		// Denotes the start of a range of commands. To be called after a command list was submitted.
		virtual void begin() = 0;

		// Denotes the end of a range of commands. To be called before a command list is submitted.
		virtual void end() = 0;

		// Directs the GPU to copy the entire content of the source buffer to the destination buffer.
		virtual void copy_buffer(Buffer const& src, Buffer& dst) = 0;

		// Directs the GPU to copy the entire content of the source image to the destination image.
		virtual void copy_image(Image const& src, Image& dst) = 0;

		// Directs the GPU to copy the entire content of the source buffer to the destination image.
		virtual void copy_buffer_to_image(Buffer const& src, Image& dst) = 0;

		// Update a region of a buffer without a staging buffer. Must be called outside of a render pass. Is more efficient when
		// the amount of data is small. Size should never exceed 65536 bytes.
		virtual void update_buffer(Buffer& dst, size_t offset, size_t size, void const* data) = 0;

		// Directs the GPU to copy size amount of bytes from the source buffer to the destination buffer at the given offsets.
		virtual void copy_buffer_region(Buffer const& src, Buffer& dst, size_t src_offset, size_t dst_offset, size_t size) = 0;

		// Directs the GPU to copy the amount of texel data specified by region from the source image to the destination image.
		virtual void copy_image_region(Image const& src, Image& dst, ImageCopyRegion const& region) = 0;
	};

	export class ComputeCommandListBase : public CopyCommandListBase
	{
	public:
		// Binds a pipeline for compute commands.
		virtual void bind_compute_pipeline(ComputePipeline const& pipeline) = 0;

		// Binds a root signature describing the layout of resources used in various compute pipelines.
		virtual void bind_compute_root_signature(RootSignature const& root_signature) = 0;

		// Binds descriptors into the layout of the currently bound compute root signature.
		virtual void bind_compute_descriptors(ArrayView<DescriptorSet> descriptor_sets) = 0;

		// Update constant data at the given offset with the given size. Only affects compute workloads.
		virtual void push_compute_constants(size_t byte_offset, size_t byte_size, void const* data) = 0;

		// Dispatch a compute shader.
		virtual void dispatch(unsigned x_count, unsigned y_count, unsigned z_count) = 0;

		// Dispatch a compute shader indirectly by having the group counts be read from the given buffer at the given offset.
		virtual void dispatch_indirect(Buffer const& buffer, size_t offset) = 0;
	};

	export class RenderCommandListBase : public ComputeCommandListBase
	{
	public:
		// Begins a render pass, binding the given render target, potentially clearing it with the given clear values. The span
		// of clear values should hold clear values corresponding to how each attachment, in the order of its render target,
		// should be cleared. Color attachments will read the clear value as a clear color, depth stencil attachments as a
		// depth-stencil clear value.
		virtual void begin_render_pass(RenderPass const&	 render_pass,
									   RenderTarget const&	 render_target,
									   ConstSpan<ClearValue> clear_values = {}) = 0;

		// Transitions the currently bound render target to its next state in the render pass.
		virtual void transition_subpass() = 0;

		// Denotes the end of a render pass.
		virtual void end_render_pass() = 0;

		// Binds a pipeline for rendering commands.
		virtual void bind_render_pipeline(RenderPipeline const& pipeline) = 0;

		// Binds a root signature describing the layout of resources used in various render pipelines.
		virtual void bind_render_root_signature(RootSignature const& root_signature) = 0;

		// Binds descriptors into the layout of the currently bound render root signature.
		virtual void bind_render_descriptors(ArrayView<DescriptorSet> descriptor_sets) = 0;

		// Update constant data at the given offset with the given size. Only affects rendering workloads.
		virtual void push_render_constants(size_t byte_offset, size_t byte_size, void const* data) = 0;

		// Bind a contiguous set of vertex buffers starting at the given first binding, each offset by an offset value with the
		// same index.
		virtual void bind_vertex_buffers(unsigned first_buffer, ArrayView<Buffer> buffers, ArrayView<size_t> byte_offsets) = 0;

		// Bind an index buffer starting at the given offset.
		virtual void bind_index_buffer(Buffer const& buffer, size_t byte_offset) = 0;

		// Set the viewport state for each color attachment in the currently bound render target.
		virtual void set_viewports(ArrayView<Viewport> viewports) = 0;

		// Set the scissor state for each color attachment in the currently bound render target.
		virtual void set_scissors(ArrayView<Rectangle> scissors) = 0;

		// Set the blend constants starting with the next draw.
		virtual void set_blend_constants(Float4 blend_constants) = 0;

		// Set the depth bounds starting with the next draw.
		virtual void set_depth_bounds(float min, float max) = 0;

		// Set the stencil reference value starting with the next draw.
		virtual void set_stencil_reference(unsigned reference_value) = 0;

		// Issue a draw call for non-indexed primitives.
		virtual void draw(unsigned vertex_count, unsigned instance_count, unsigned first_vertex, unsigned first_instance) = 0;

		// Issue a draw call for indexed primitives.
		virtual void draw_indexed(unsigned index_count,
								  unsigned instance_count,
								  unsigned first_index,
								  int	   vertex_offset,
								  unsigned first_instance) = 0;

		// Issue non-indexed draw calls indirectly by having the draw counts be read from the given buffer at the given offset.
		virtual void draw_indirect(Buffer const& buffer, size_t offset, unsigned draws) = 0;

		// Issue indexed draw calls indirectly by having the draw counts be read from the given buffer at the given offset.
		virtual void draw_indexed_indirect(Buffer const& buffer, size_t offset, unsigned draws) = 0;
	};

	export template<CommandType TYPE>
	using CommandListBase = std::conditional_t<
		TYPE == CommandType::Render,
		RenderCommandListBase,
		std::conditional_t<TYPE == CommandType::Compute,
						   ComputeCommandListBase,
						   std::conditional_t<TYPE == CommandType::Copy, CopyCommandListBase, void>>>;
}
