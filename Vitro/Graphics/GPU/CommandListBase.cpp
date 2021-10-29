﻿module;
#include <span>
export module vt.Graphics.CommandListBase;

import vt.Core.Array;
import vt.Core.FixedList;
import vt.Core.Rectangle;
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
	export enum CommandType : uint8_t {
		Copy,
		Compute,
		Render,
	};

	export union ClearValue
	{
		Float4 color = {};
		struct
		{
			float	 depth;
			unsigned stencil;
		};
	};

	export class CopyCommandListBase
	{
	public:
		virtual ~CopyCommandListBase() = default;

		virtual CommandListHandle handle() = 0;
		virtual void			  reset()  = 0;
		virtual void			  begin()  = 0;
		virtual void			  end()	   = 0;
	};

	export class ComputeCommandListBase : public CopyCommandListBase
	{
	public:
		virtual void bind_compute_pipeline(ComputePipeline const& pipeline)							= 0;
		virtual void bind_compute_root_signature(RootSignature const& root_signature)				= 0;
		virtual void bind_compute_descriptors(ArrayView<DescriptorSet> descriptor_sets)				= 0;
		virtual void push_compute_constants(size_t byte_offset, size_t byte_size, void const* data) = 0;
		virtual void dispatch(unsigned x_count, unsigned y_count, unsigned z_count)					= 0;
		virtual void dispatch_indirect(Buffer const& buffer, size_t offset)							= 0;
	};

	export class RenderCommandListBase : public ComputeCommandListBase
	{
	public:
		virtual void begin_render_pass(RenderPass const&	 render_pass,
									   RenderTarget const&	 render_target,
									   ConstSpan<ClearValue> clear_values = {})											   = 0;
		virtual void transition_subpass()																				   = 0;
		virtual void end_render_pass()																					   = 0;
		virtual void bind_render_pipeline(RenderPipeline const& pipeline)												   = 0;
		virtual void bind_render_root_signature(RootSignature const& root_signature)									   = 0;
		virtual void bind_render_descriptors(ArrayView<DescriptorSet> descriptor_sets)									   = 0;
		virtual void push_render_constants(size_t byte_offset, size_t byte_size, void const* data)						   = 0;
		virtual void bind_vertex_buffers(unsigned first_buffer, ArrayView<Buffer> buffers, ArrayView<size_t> byte_offsets) = 0;
		virtual void bind_index_buffer(Buffer const& buffer, size_t byte_offset)										   = 0;
		virtual void set_viewports(ArrayView<Viewport> viewports)														   = 0;
		virtual void set_scissors(ArrayView<Rectangle> scissors)														   = 0;
		virtual void set_blend_constants(Float4 blend_constants)														   = 0;
		virtual void set_depth_bounds(float min, float max)																   = 0;
		virtual void set_stencil_reference(unsigned reference_value)													   = 0;
		virtual void draw(unsigned vertex_count, unsigned instance_count, unsigned first_vertex, unsigned first_instance)  = 0;
		virtual void draw_indexed(unsigned index_count,
								  unsigned instance_count,
								  unsigned first_index,
								  int	   vertex_offset,
								  unsigned first_instance)																   = 0;
		virtual void draw_indirect(Buffer const& buffer, size_t offset, unsigned draws)									   = 0;
		virtual void draw_indexed_indirect(Buffer const& buffer, size_t offset, unsigned draws)							   = 0;
	};

	export template<CommandType TYPE>
	using CommandListBase = std::conditional_t<
		TYPE == CommandType::Render,
		RenderCommandListBase,
		std::conditional_t<TYPE == CommandType::Compute,
						   ComputeCommandListBase,
						   std::conditional_t<TYPE == CommandType::Copy, CopyCommandListBase, void>>>;
}
