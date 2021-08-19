module;
#include <array>
#include <new>
export module Vitro.Graphics.ForwardRenderer;

import Vitro.Graphics.CommandList;
import Vitro.Graphics.Device;
import Vitro.Graphics.FrameContext;
import Vitro.Graphics.SwapChain;
import Vitro.Trace.Log;

namespace vt
{
	export class ForwardRenderer
	{
	public:
		ForwardRenderer(Device& device) : device(device), context(2, device)
		{}

		void draw(SwapChain& swapChain)
		{
			auto& cmd = context->cmd;

			auto& renderTarget = swapChain->acquireRenderTarget();
			cmd->reset();

			cmd->begin();
			cmd->end();

			std::array cmdLists {cmd->handle()};
			device->submitRenderCommands(cmdLists);
			swapChain->present();

			context.moveToNextFrame();
		}

	private:
		struct FrameResources
		{
			RenderCommandList cmd;
		};

		Device&						 device;
		FrameContext<FrameResources> context;
	};
}
