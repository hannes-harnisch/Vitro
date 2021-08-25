module;
#include <span>
export module vt.Graphics.CommandListBase;

import vt.Core.FixedList;
import vt.Core.Rectangle;
import vt.Core.Vector;
import vt.Graphics.Driver;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderPassInfo;
import vt.Graphics.RenderTarget;
import vt.Graphics.Resource;
import vt.Graphics.RootSignature;

namespace vt
{
	export enum CommandType : unsigned char {
		Copy,
		Compute,
		Render,
	};

	export union ClearValue
	{
		Float4 color = {};
		struct
		{
			float		  depth;
			unsigned char stencil;
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
		virtual void bind_compute_pipeline(Pipeline const& pipeline)						  = 0;
		virtual void bind_compute_root_signature(RootSignature const& root_signature)		  = 0;
		virtual void push_compute_constants(void const* data, unsigned size, unsigned offset) = 0;
		virtual void dispatch(unsigned x_count, unsigned y_count, unsigned z_count)			  = 0;
	};

	export class RenderCommandListBase : public ComputeCommandListBase
	{
	public:
		virtual void bind_render_pipeline(Pipeline const& pipeline)														  = 0;
		virtual void bind_render_root_signature(RootSignature const& root_signature)									  = 0;
		virtual void push_render_constants(void const* data, unsigned size, unsigned offset)							  = 0;
		virtual void begin_render_pass(RenderPass const&		   render_pass,
									   RenderTarget const&		   render_target,
									   std::span<ClearValue const> clear_values = {})									  = 0;
		virtual void transition_to_next_subpass()																		  = 0;
		virtual void end_render_pass()																					  = 0;
		virtual void bind_vertex_buffers(unsigned				   first_buffer,
										 std::span<Buffer const>   buffers,
										 std::span<unsigned const> byte_offsets)										  = 0;
		virtual void bind_index_buffer(Buffer const& buffer, unsigned byte_offset)										  = 0;
		virtual void bind_primitive_topology(PrimitiveTopology topology)												  = 0;
		virtual void bind_viewports(std::span<Viewport const> viewports)												  = 0;
		virtual void bind_scissors(std::span<Rectangle const> scissors)													  = 0;
		virtual void draw(unsigned vertex_count, unsigned instance_count, unsigned first_vertex, unsigned first_instance) = 0;
		virtual void draw_indexed(unsigned index_count,
								  unsigned instance_count,
								  unsigned first_index,
								  int	   vertex_offset,
								  unsigned first_instance)																  = 0;
	};
}
