module;
#include <vector>
export module vt.Graphics.ForwardRenderer;

import vt.App.EventListener;
import vt.App.Input;
import vt.App.WindowEvent;
import vt.Core.Rect;
import vt.Core.Tick;
import vt.Core.Transform;
import vt.Core.Vector;
import vt.Graphics.AssetResource;
import vt.Graphics.Camera;
import vt.Graphics.CommandList;
import vt.Graphics.AbstractCommandList;
import vt.Graphics.DeletionQueue;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;
import vt.Graphics.RendererBase;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RingBuffer;
import vt.Graphics.RootSignature;
import vt.Trace.Log;

namespace vt
{
	export class ForwardRenderer : public RendererBase, public EventListener
	{
	public:
		ForwardRenderer(Device& device, Extent shared_render_target_size, ImageFormat shared_render_target_format) :
			RendererBase(device),
			cam({-3, 0, -3}, {3, 0, 3}, project_perspective(0.4f * 3.14f, shared_render_target_size, 1.0f, 1000.f)),
			final_render_pass(make_final_render_pass(shared_render_target_format)),
			context(device)
		{
			initialize_root_signature_and_pipeline();
			initialize_vertex_and_index_buffers();
			initialize_depth_image(shared_render_target_size);

			register_event_handlers<&ForwardRenderer::on_mouse_move>();
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

			ClearValue clear_value[] {
				ClearValue {
					.color = clear_color,
				},
				ClearValue {
					.depth = 1.0f,
				},
			};
			cmd->begin_render_pass(final_render_pass, render_target, clear_value);
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

			update_cam(tick);
			auto& vp = cam.get_view_projection();
			cmd->push_render_constants(sizeof triangle_color, sizeof vp, &vp);

			size_t offset = 0;
			cmd->bind_vertex_buffers(0, vertex_buffers[0], offset);
			cmd->bind_index_buffer(index_buffers[0], 0);

			cmd->draw_indexed(36, 1, 0, 0, 0);
			cmd->end_render_pass();
			cmd->end();

			auto cmd_list = cmd->get_handle();
			context.move_to_next_frame();
			return {cmd_list};
		}

		SharedRenderTargetSpecification specify_shared_render_target() const override
		{
			return {
				.depth_stencil_attachment = &depth_images[0],
				.render_pass			  = final_render_pass,
				.shared_img_dst_index	  = 0,
			};
		}

		void on_render_target_resize(Extent size) override
		{
			depth_images.clear();
			initialize_depth_image(size);
		}

	private:
		Camera							 cam;
		RenderPass						 final_render_pass;
		std::vector<DescriptorSetLayout> descriptor_set_layouts;
		std::vector<RootSignature>		 root_signatures;
		std::vector<RenderPipeline>		 render_pipelines;
		std::vector<Buffer>				 vertex_buffers;
		std::vector<Buffer>				 index_buffers;
		std::vector<Image>				 depth_images;
		float							 time = 0;

		struct FrameResources
		{
			RenderCommandList render_cmd_list;
			DeletionQueue	  deletion_queue;

			FrameResources(Device& device) : render_cmd_list(device->make_render_command_list())
			{}
		};
		RingBuffer<FrameResources> context;

		RenderPass make_final_render_pass(ImageFormat swap_chain_format)
		{
			Subpass subpass {
				.output_attachments = {0, 1},
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
					AttachmentSpecification {
						.format			= ImageFormat::D32Float,
						.load_op		= ImageLoadOp::Clear,
						.store_op		= ImageStoreOp::Store,
						.initial_layout = ImageLayout::Undefined,
						.final_layout	= ImageLayout::DepthStencilAttachment,
					},
				},
				.subpasses = subpass,
			};
			return device->make_render_pass(spec);
		}

		void initialize_root_signature_and_pipeline()
		{
			RootSignatureSpecification const root_sig_spec {
				.push_constants_byte_size  = sizeof(Float4) + sizeof(Float4x4),
				.push_constants_visibility = ShaderStage::All,
			};
			auto& sig = root_signatures.emplace_back(device->make_root_signature(root_sig_spec));

			auto vertex_shader	 = device->make_shader("Cube.vert." VT_SHADER_EXTENSION);
			auto fragment_shader = device->make_shader("Cube.frag." VT_SHADER_EXTENSION);

			RenderPipelineSpecification const pipe_spec {
				.root_signature	 = sig,
				.render_pass	 = final_render_pass,
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
					.cull_mode	   = CullMode::Back,
					.winding_order = WindingOrder::Clockwise,
				},
				.blend {
					.attachment_states = {1},
				},
			};
			render_pipelines = device->make_render_pipelines(pipe_spec);
		}

		void initialize_vertex_and_index_buffers()
		{
			struct Vertex
			{
				Float4 position;
				Float4 color;
			};
			Vertex vertices[] {
				{{-1, -1, -1, 1}, {0, 0, 0, 1}}, {{-1, 1, -1, 1}, {0, 1, 0, 1}}, {{1, 1, -1, 1}, {1, 1, 0, 1}},
				{{1, -1, -1, 1}, {1, 0, 0, 1}},	 {{-1, -1, 1, 1}, {0, 0, 1, 1}}, {{-1, 1, 1, 1}, {0, 1, 1, 1}},
				{{1, 1, 1, 1}, {1, 1, 1, 1}},	 {{1, -1, 1, 1}, {1, 0, 1, 1}},
			};
			uint32_t indices[] {0, 1, 2, 0, 2, 3, 4, 6, 5, 4, 7, 6, 4, 5, 1, 4, 1, 0,
								3, 2, 6, 3, 6, 7, 1, 5, 6, 1, 6, 2, 4, 0, 3, 4, 3, 7};

			BufferSpecification const staging_buffer_spec {
				.size	= sizeof vertices + sizeof indices,
				.stride = 1,
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
			std::memcpy((char*)dst + sizeof vertices, indices, sizeof indices);
			device->unmap(stage);

			BufferSpecification const index_buffer_spec {
				.size	= sizeof indices,
				.stride = sizeof(uint32_t),
				.usage	= BufferUsage::CopyDst | BufferUsage::Index,
			};
			auto& index_buffer = index_buffers.emplace_back(device->make_buffer(index_buffer_spec));

			auto cmd = device->make_copy_command_list();
			cmd->reset();
			cmd->begin();
			cmd->copy_buffer_region(stage, vertex_buffer, 0, 0, sizeof vertices);
			cmd->copy_buffer_region(stage, index_buffer, sizeof vertices, 0, sizeof indices);
			cmd->end();

			auto list  = cmd->get_handle();
			auto token = device->submit_copy_commands(list);
			device->wait_for_workload(token);
		}

		void initialize_depth_image(Extent size)
		{
			ImageSpecification const spec {
				.expanse   = {size.width, size.height},
				.dimension = ImageDimension::Image2D,
				.format	   = ImageFormat::D32Float,
				.mip_count = 1,
				.usage	   = ImageUsage::DepthStencil,
			};
			depth_images.emplace_back(device->make_image(spec));
		}

		void update_cam(Tick tick)
		{
			float move_speed = 5 * tick;

			if(Input::is_down(KeyCode::A))
				cam.translate({-move_speed, 0, 0});
			if(Input::is_down(KeyCode::D))
				cam.translate({move_speed, 0, 0});

			if(Input::is_down(KeyCode::Q))
				cam.translate({0, -move_speed, 0});
			if(Input::is_down(KeyCode::E))
				cam.translate({0, move_speed, 0});

			if(Input::is_down(KeyCode::S))
				cam.translate({0, 0, -move_speed});
			if(Input::is_down(KeyCode::W))
				cam.translate({0, 0, move_speed});

			if(Input::is_down(KeyCode::R))
				cam.set_position({-3, 0, -3});

			if(Input::is_down(KeyCode::F))
				cam.roll(-tick);
			if(Input::is_down(KeyCode::G))
				cam.roll(tick);
		}

		void on_mouse_move(MouseMoveEvent& event)
		{
			cam.yaw(radians(0.25f * event.direction.x));
			cam.pitch(radians(0.25f * event.direction.y));
		}
	};
}
