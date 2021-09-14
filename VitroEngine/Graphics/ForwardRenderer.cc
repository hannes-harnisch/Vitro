module;
#include <array>
#include <new>
#include <span>
export module vt.Graphics.ForwardRenderer;

import vt.Graphics.CommandList;
import vt.Graphics.CommandListBase;
import vt.Graphics.Device;
import vt.Graphics.DeviceBase;
import vt.Graphics.FrameContext;
import vt.Graphics.RenderPass;
import vt.Graphics.SwapChain;
import vt.Trace.Log;

namespace vt
{
	export class ForwardRenderer
	{
	public:
		ForwardRenderer(Device& device, RenderPass const& present_pass) :
			device(device), present_pass(present_pass), context(FrameBufferCount, device)
		{}

		void draw(SwapChain& swap_chain)
		{
			auto& cmd = context->cmd;

			device->wait_for_render_workload(context->frame_finished);
			auto& swap_chain_render_target = swap_chain->acquire_render_target();
			cmd->reset();

			cmd->begin();

			std::array clear_values {
				ClearValue {
					.color = {0.3, 0.5, 0.3, 1},
				},
			};
			cmd->begin_render_pass(present_pass, swap_chain_render_target, clear_values);
			cmd->end_render_pass();
			cmd->end();

			std::array cmd_lists {cmd->handle()};
			context->frame_finished = device->submit_render_commands(cmd_lists);
			swap_chain->present();

			context.move_to_next_frame();
		}

	private:
		static constexpr unsigned FrameBufferCount = 2;

		Device&			  device;
		RenderPass const& present_pass;

		struct FrameResources
		{
			RenderCommandList cmd;
			Receipt			  frame_finished;
		};
		FrameContext<FrameResources> context;
	};
}
