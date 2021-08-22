module;
#include <array>
#include <new>
#include <span>
export module Vitro.Graphics.ForwardRenderer;

import Vitro.Graphics.CommandList;
import Vitro.Graphics.CommandListBase;
import Vitro.Graphics.Device;
import Vitro.Graphics.DeviceBase;
import Vitro.Graphics.FrameContext;
import Vitro.Graphics.RenderPass;
import Vitro.Graphics.SwapChain;
import Vitro.Trace.Log;

namespace vt
{
	export class ForwardRenderer
	{
	public:
		ForwardRenderer(Device& device, RenderPass const& presentPass) :
			device(device), presentPass(presentPass), context(FrameBufferCount, device)
		{}

		void draw(SwapChain& swapChain)
		{
			auto& cmd = context->cmd;

			device->waitForRenderWorkload(context->frameFinished);
			auto& swapChainRenderTarget = swapChain->acquireRenderTarget();
			cmd->reset();

			cmd->begin();

			std::array clearValues {
				ClearValue {{1, 0.5, 0, 1}},
			};
			cmd->beginRenderPass(presentPass, swapChainRenderTarget, clearValues);
			cmd->endRenderPass();
			cmd->end();

			std::array cmdLists {cmd->handle()};
			context->frameFinished = device->submitRenderCommands(cmdLists);
			swapChain->present();

			context.moveToNextFrame();
		}

	private:
		struct FrameResources
		{
			RenderCommandList cmd;
			Receipt			  frameFinished;
		};

		static constexpr unsigned FrameBufferCount = 2;

		Device&						 device;
		RenderPass const&			 presentPass;
		FrameContext<FrameResources> context;
	};
}
