module;
#include <array>
#include <new>
#include <span>
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
import vt.Graphics.RenderPass;
import vt.Graphics.RootSignature;
import vt.Graphics.SwapChain;
import vt.Trace.Log;

namespace vt
{
	export class ForwardRenderer
	{
	public:
		ForwardRenderer(Device& device, RenderPass const& present_pass) :
			device(device), present_pass(present_pass), context(FramesBuffered, device)
		{
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
			auto& swap_chain_render_target = swap_chain->acquire_render_target();

			auto& cmd = context->command_list;
			cmd->reset();
			cmd->begin();

			clear_color += Float4 {inc, inc, inc};
			if(clear_color.r < 0.0f || clear_color.r > 1.0f)
				inc = -inc;

			ClearValue clear_value {
				.color = clear_color,
			};
			cmd->begin_render_pass(present_pass, swap_chain_render_target, clear_value);
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

		Device&							 device;
		RenderPass const&				 present_pass;
		std::vector<DescriptorSetLayout> descriptor_set_layouts;
		std::vector<RootSignature>		 root_signatures;
		std::vector<RenderPipeline>		 render_pipelines;

		struct FrameResources
		{
			RenderCommandList command_list;
			SyncValue		  frame_finished;
		};
		FrameContext<FrameResources> context;
	};
}
