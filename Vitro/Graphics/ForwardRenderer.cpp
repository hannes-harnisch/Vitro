module;
#include <vector>
export module vt.Graphics.ForwardRenderer;

import vt.Core.Matrix;
import vt.Core.Rectangle;
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
		ForwardRenderer(Device& device) : RendererBase(device), present_pass(make_present_pass()), context(device)
		{
			RootSignatureSpecification const root_sig_spec {
				.push_constants_byte_size = sizeof(Float4),
			};
			auto& sig = root_signatures.emplace_back(device->make_root_signature(root_sig_spec));

			auto vertex_shader	 = device->make_shader("Triangle.vert.cso");
			auto fragment_shader = device->make_shader("Triangle.frag.cso");

			RenderPipelineSpecification const pipe_spec {
				.root_signature		= sig,
				.render_pass		= present_pass,
				.vertex_shader		= vertex_shader,
				.fragment_shader	= &fragment_shader,
				.primitive_topology = PrimitiveTopology::TriangleList,
				.subpass_index		= 0,
				.rasterizer {
					.cull_mode	= CullMode::None,
					.front_face = FrontFace::CounterClockwise,
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
		}

	protected:
		std::vector<CommandListHandle> render(RenderTarget const& render_target) override
		{
			context->deletion_queue.delete_all();

			auto& cmd = context->command_list;
			cmd->reset();
			cmd->begin();

			Float4 clear_color = 0.5f + 0.5f * cos(time++ / 1000.0f + Float3 {0, 2, 4});
			clear_color.a	   = 1;

			ClearValue clear_value {
				.color = clear_color,
			};
			cmd->begin_render_pass(present_pass, render_target, clear_value);
			cmd->bind_render_root_signature(root_signatures.front());
			cmd->bind_render_pipeline(render_pipelines.front());

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

			cmd->draw(3, 1, 0, 0);
			cmd->end_render_pass();
			cmd->end();

			auto cmd_list = cmd->handle();
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
		unsigned time = 0;

		RenderPass						 present_pass;
		std::vector<DescriptorSetLayout> descriptor_set_layouts;
		std::vector<RootSignature>		 root_signatures;
		std::vector<RenderPipeline>		 render_pipelines;

		struct FrameResources
		{
			RenderCommandList command_list;
			DeletionQueue	  deletion_queue;

			FrameResources(Device& device) : command_list(device->make_render_command_list())
			{}
		};
		RingBuffer<FrameResources> context;

		RenderPass make_present_pass()
		{
			Subpass subpass {
				.output_refs {
					AttachmentReference {
						.index		 = 0,
						.used_layout = ImageLayout::ColorAttachment,
					},
				},
			};
			RenderPassSpecification const spec {
				.attachments {
					AttachmentSpecification {
						.format			= ImageFormat::R8_G8_B8_A8_UNorm,
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
