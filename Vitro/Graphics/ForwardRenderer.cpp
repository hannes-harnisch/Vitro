module;
#include <vector>
export module vt.Graphics.ForwardRenderer;

import vt.Core.Matrix;
import vt.Core.Vector;
import vt.Graphics.AssetResource;
import vt.Graphics.CommandList;
import vt.Graphics.CommandListBase;
import vt.Graphics.DescriptorBinding;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;
import vt.Graphics.DeviceBase;
import vt.Graphics.FrameContext;
import vt.Graphics.Handle;
import vt.Graphics.RendererBase;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RootSignature;
import vt.Graphics.SwapChain;
import vt.Trace.Log;

namespace vt
{
	export class ForwardRenderer : public RendererBase
	{
	public:
		ForwardRenderer(Device& device) : device(device), present_pass(make_present_pass()), context(FramesBuffered, device)
		{
			make_swap_chain_targets(swap_chain);

			RootSignatureSpecification const root_sig_spec {
				.push_constants_byte_size = 2 * sizeof(Float4x4),
			};
			auto& sig = root_signatures.emplace_back(device, root_sig_spec);

			/*RenderPipelineSpecification const pipe_spec {
				.root_signature = sig,
				.render_pass	= present_pass,
			};
			render_pipelines.emplace_back(device, pipe_spec);*/
		}

		void draw(SwapChain& swap_chain)
		{
			device->wait_for_workload(context->frame_finished);
			unsigned img_index = swap_chain->get_current_image_index();

			auto& cmd = context->command_list;
			cmd->reset();
			cmd->begin();

			clear_color += Float4 {inc, inc, inc};
			if(clear_color.r < 0.0f || clear_color.r > 1.0f)
				inc = -inc;

			ClearValue clear_value {
				.color = clear_color,
			};
			cmd->begin_render_pass(present_pass, swap_chain_targets[img_index], clear_value);
			cmd->end_render_pass();
			cmd->end();

			auto cmd_list = cmd->handle();

			context->frame_finished = device->submit_render_commands(cmd_list);
			swap_chain->present();

			context.move_to_next_frame();
		}

	private:
		static constexpr unsigned FramesBuffered = 2;

		float  inc		   = 0.0001f;
		Float4 clear_color = {0, 0, 0, 1};

		Device&	   device;
		RenderPass present_pass;

		std::vector<DescriptorSetLayout> descriptor_set_layouts;
		std::vector<RootSignature>		 root_signatures;
		std::vector<RenderPipeline>		 render_pipelines;

		struct FrameResources
		{
			RenderCommandList command_list;
			SyncValue		  frame_finished;
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
			return {device, spec};
		}

		void make_swap_chain_targets(SwapChain& swap_chain)
		{
			unsigned const count = swap_chain->get_buffer_count();
			for(unsigned i = 0; i < count; ++i)
			{
				RenderTargetSpecification const spec {
					.render_pass		  = present_pass,
					.swap_chain			  = &swap_chain,
					.swap_chain_src_index = i,
					.swap_chain_dst_index = 0,
				};
				swap_chain_targets.emplace_back(device, spec);
			}
		}
	};
}
