module;
#include <array>
#include <new>
#include <span>
#include <vector>
export module vt.Graphics.ForwardRenderer;

import vt.Core.Vector;
import vt.Graphics.CommandList;
import vt.Graphics.CommandListBase;
import vt.Graphics.DescriptorSetLayout;
import vt.Graphics.Device;
import vt.Graphics.DeviceBase;
import vt.Graphics.FrameContext;
import vt.Graphics.Handle;
import vt.Graphics.RenderPass;
import vt.Graphics.Resource;
import vt.Graphics.RootSignature;
import vt.Graphics.SwapChain;
import vt.Trace.Log;

namespace vt
{
	export class ForwardRenderer
	{
	public:
		ForwardRenderer(Device& device, RenderPass const& present_pass) :
			device(device), present_pass(present_pass), context(FrameBufferCount, device)
		{
			// descriptor_set_layouts.emplace_back(device);
		}

		void draw(SwapChain& swap_chain)
		{
			device->wait_for_workload(context->frame_finished);
			auto& swap_chain_render_target = swap_chain->acquire_render_target();
			auto& cmd					   = context->cmd;
			cmd->reset();

			cmd->begin();

			clear_color += Float4 {inc, inc, inc};
			if(clear_color.r < 0.0f || clear_color.r > 1.0f)
				inc = -inc;

			std::array clear_values {
				ClearValue {
					.color = clear_color,
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

		float  inc = 0.0001f;
		Float4 clear_color {0, 0, 0, 1};

		Device&							 device;
		RenderPass const&				 present_pass;
		std::vector<DescriptorSetLayout> descriptor_set_layouts;
		std::vector<RootSignature>		 root_signatures;
		std::vector<RenderPipeline>		 render_pipelines;

		struct FrameResources
		{
			RenderCommandList cmd;
			SyncValue		  frame_finished;
		};
		FrameContext<FrameResources> context;
	};
}
