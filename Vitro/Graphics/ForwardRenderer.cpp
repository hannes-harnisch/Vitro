module;
#include <vector>
export module vt.Graphics.ForwardRenderer;

import vt.Core.Matrix;
import vt.Core.Rect;
import vt.Core.Tick;
import vt.Core.Vector;
import vt.Graphics.AssetResource;
import vt.Graphics.CommandList;
import vt.Graphics.CommandListBase;
import vt.Graphics.DeletionQueue;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;
import vt.Graphics.RendererBase;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RingBuffer;
import vt.Graphics.RootSignature;

namespace vt
{
	export class ForwardRenderer : public RendererBase
	{
	public:
		ForwardRenderer(Device& device, ImageFormat swap_chain_format) :
			RendererBase(device), present_pass(make_present_pass(swap_chain_format)), context(device)
		{
			RootSignatureSpecification const root_sig_spec {
				.push_constants_byte_size  = sizeof(Float4),
				.push_constants_visibility = ShaderStage::Render,
			};
			auto& sig = root_signatures.emplace_back(device->make_root_signature(root_sig_spec));

			auto vertex_shader	 = device->make_shader("Triangle.vert." VT_SHADER_EXTENSION);
			auto fragment_shader = device->make_shader("Triangle.frag." VT_SHADER_EXTENSION);

			RenderPipelineSpecification const pipe_spec {
				.root_signature	 = sig,
				.render_pass	 = present_pass,
				.vertex_shader	 = vertex_shader,
				.fragment_shader = &fragment_shader,
				.vertex_buffer_bindings {
					VertexBufferBinding {
						.attributes {
							VertexAttribute {
								.type = VertexDataType::Position,
							},
							VertexAttribute {
								.type = VertexDataType::Color,
							},
						},
					},
				},
				.primitive_topology = PrimitiveTopology::TriangleList,
				.subpass_index		= 0,
				.rasterizer {
					.cull_mode	   = CullMode::None,
					.winding_order = WindingOrder::CounterClockwise,
				},
				.depth_stencil {
					.enable_depth_test	 = false,
					.enable_stencil_test = false,
				},
				.blend {
					.attachment_states = {1},
				},
			};
			render_pipelines = device->make_render_pipelines(pipe_spec);

			// Testing vertex buffers
			struct Vertex
			{
				Float4 position;
				Float4 color;
			};
			Vertex vertices[] {
				{{0, 0.5, 0.5, 1}, {1, 0, 0, 1}},
				{{0.5, -0.5, 0.5, 1}, {0, 1, 0, 1}},
				{{-0.5, -0.5, 0.5, 1}, {0, 0, 1, 1}},
			};

			BufferSpecification const staging_buffer_spec {
				.size	= sizeof vertices,
				.stride = sizeof(Vertex),
				.usage	= BufferUsage::CopySrc | BufferUsage::Upload,
			};
			auto stage = device->make_buffer(staging_buffer_spec);

			BufferSpecification const vertex_buffer_spec {
				.size	= sizeof vertices,
				.stride = sizeof(Vertex),
				.usage	= BufferUsage::CopyDst | BufferUsage::Vertex,
			};
			auto& vertex_buffer = vertex_buffers.emplace_back(device->make_buffer(vertex_buffer_spec));

			auto dst = device->map(stage);
			std::memcpy(dst, vertices, sizeof vertices);
			device->unmap(stage);

			auto cmd = device->make_copy_command_list();
			cmd->reset();
			cmd->begin();
			cmd->copy_buffer(stage, vertex_buffer);
			cmd->end();

			auto list  = cmd->get_handle();
			auto token = device->submit_copy_commands(list);
			device->wait_for_workload(token);
		}

	protected:
		SmallList<CommandListHandle> render(Tick tick, RenderTarget const& render_target) override
		{
			auto& current = context.current();

			current.deletion_queue.delete_all();

			auto& cmd = current.render_cmd_list;
			cmd->reset();
			cmd->begin();

			Float4 clear_color = 0.5f + 0.5f * cos(time / 1000.0f + Float3 {0, 2, 4});
			clear_color.a	   = 1;
			time += tick * 1000;

			ClearValue clear_value {
				.color = clear_color,
			};
			cmd->begin_render_pass(present_pass, render_target, clear_value);
			cmd->bind_render_root_signature(root_signatures[0]);
			cmd->bind_render_pipeline(render_pipelines[0]);

			Viewport viewport {
				.width	= static_cast<float>(render_target.get_width()),
				.height = static_cast<float>(render_target.get_height()),
			};
			cmd->set_viewports(viewport);

			Rectangle scissor {
				.width	= render_target.get_width(),
				.height = render_target.get_height(),
			};
			cmd->set_scissors(scissor);

			Float4 triangle_color = 0.5f + 0.5f * cos(time / 1000.0f + Float3 {3, 1.5, 0});
			triangle_color.a	  = 1;
			cmd->push_render_constants(0, sizeof triangle_color, &triangle_color);

			size_t offset = 0;
			cmd->bind_vertex_buffers(0, vertex_buffers[0], offset);

			cmd->draw(3, 1, 0, 0);
			cmd->end_render_pass();
			cmd->end();

			auto cmd_list = cmd->get_handle();
			context.move_to_next_frame();
			return {cmd_list};
		}

		SharedRenderTargetSpecification get_shared_render_target_specification() const override
		{
			return {
				.render_pass		  = present_pass,
				.shared_img_dst_index = 0,
			};
		}

		void on_render_target_resize(unsigned, unsigned) override
		{}

	private:
		RenderPass						 present_pass;
		std::vector<DescriptorSetLayout> descriptor_set_layouts;
		std::vector<RootSignature>		 root_signatures;
		std::vector<RenderPipeline>		 render_pipelines;
		std::vector<Buffer>				 vertex_buffers;
		float							 time = 0;

		struct FrameResources
		{
			RenderCommandList render_cmd_list;
			DeletionQueue	  deletion_queue;

			FrameResources(Device& device) : render_cmd_list(device->make_render_command_list())
			{}
		};
		RingBuffer<FrameResources> context;

		RenderPass make_present_pass(ImageFormat swap_chain_format)
		{
			Subpass subpass {
				.output_attachments = {0},
			};
			RenderPassSpecification const spec {
				.attachments {
					AttachmentSpecification {
						.format			= swap_chain_format,
						.load_op		= ImageLoadOp::Clear,
						.store_op		= ImageStoreOp::Store,
						.initial_layout = ImageLayout::Undefined,
						.final_layout	= ImageLayout::Presentable,
					},
				},
				.subpasses = subpass,
			};
			return device->make_render_pass(spec);
		}
	};
}
