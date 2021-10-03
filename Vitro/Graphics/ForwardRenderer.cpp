module;
#include <fstream>
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
import vt.Graphics.FrameContext;
import vt.Graphics.Handle;
import vt.Graphics.RendererBase;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RootSignature;
import vt.Graphics.SwapChain;

namespace vt
{
	export class ForwardRenderer : public RendererBase
	{
	public:
		ForwardRenderer(Device& device) :
			RendererBase(device), present_pass(make_present_pass()), context(FRAMES_BUFFERED, device)
		{
			RenderTargetSpecification const render_target_spec {
				.render_pass		  = &present_pass,
				.swap_chain_dst_index = 0,
			};
			register_render_target_specification(render_target_spec);

			RootSignatureSpecification const root_sig_spec {
				.push_constants_byte_size = sizeof(Float4),
			};
			auto& sig = root_signatures.emplace_back(device->make_root_signature(root_sig_spec));

			std::ifstream	  vert_file("Triangle.vert.cso", std::ios::binary);
			std::ifstream	  frag_file("Triangle.frag.cso", std::ios::binary);
			std::vector<char> vert_shader(std::istreambuf_iterator<char>(vert_file), {});
			std::vector<char> frag_shader(std::istreambuf_iterator<char>(frag_file), {});

			RenderPipelineSpecification const pipe_spec {
				.root_signature			  = sig,
				.render_pass			  = present_pass,
				.vertex_shader_bytecode	  = vert_shader,
				.fragment_shader_bytecode = frag_shader,
				.primitive_topology		  = PrimitiveTopology::TriangleList,
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

		void draw(SwapChain& swap_chain) override
		{
			device->wait_for_workload(context->frame_finished);
			context->deletion_queue.delete_all();

			unsigned img_index		   = swap_chain->get_current_image_index();
			auto&	 swap_chain_target = get_swap_chain_target(swap_chain, img_index);

			auto& cmd = context->command_list;
			cmd->reset();
			cmd->begin();
			cmd->bind_primitive_topology(PrimitiveTopology::TriangleList);

			Float4 clear_color = 0.5f + 0.5f * cos(time++ / 1000.0f + Float3 {0, 2, 4});
			clear_color.a	   = 1;

			ClearValue clear_value {
				.color = clear_color,
			};
			cmd->begin_render_pass(present_pass, swap_chain_target, clear_value);
			cmd->bind_render_root_signature(root_signatures.front());
			cmd->bind_render_pipeline(render_pipelines.front());

			Viewport viewport {
				.width	= 1000,
				.height = 1000,
			};
			cmd->bind_viewports(viewport);

			Rectangle scissor {
				.width	= 1000,
				.height = 1000,
			};
			cmd->bind_scissors(scissor);

			Float4 triangle_color = 0.5f + 0.5f * cos(time / 1000.0f + Float3 {3, 1.5, 0});
			triangle_color.a	  = 1;
			cmd->push_render_constants(0, sizeof triangle_color, &triangle_color);

			cmd->draw(3, 1, 0, 0);
			cmd->end_render_pass();
			cmd->end();

			auto cmd_list = cmd->handle();

			context->frame_finished = device->submit_render_commands(cmd_list);
			swap_chain->present();

			context.move_to_next_frame();
		}

	private:
		static constexpr unsigned FRAMES_BUFFERED = 2;

		unsigned time = 0;

		RenderPass						 present_pass;
		std::vector<DescriptorSetLayout> descriptor_set_layouts;
		std::vector<RootSignature>		 root_signatures;
		std::vector<RenderPipeline>		 render_pipelines;

		struct FrameResources
		{
			RenderCommandList command_list;
			SyncValue		  frame_finished;
			DeletionQueue	  deletion_queue;
		};
		FrameContext<FrameResources> context;

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
