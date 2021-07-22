module;
#include <new>
export module Vitro.Graphics.ForwardRenderer;

import Vitro.Graphics.CommandList;
import Vitro.Graphics.Device;
import Vitro.Graphics.DeferredDeleter;
import Vitro.Graphics.SwapChain;
import Vitro.Trace.Log;

namespace vt
{
	export class ForwardRenderer
	{
	public:
		ForwardRenderer(Device& device) : cmd(device)
		{}

		void draw(SwapChain& swapChain)
		{
			// Log().info(swapChain->getNextRenderTargetIndex());
			//	swapChain->present();

			frameResourceIndex = (frameResourceIndex + 1) % MaxFramesInFlight;
		}

	private:
		static constexpr unsigned MaxFramesInFlight = 2;

		DeferredDeleter deferredDeleter;
		RenderCommandList cmd;
		unsigned frameResourceIndex = 0;
	};
}
