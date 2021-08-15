module;
#include <new>
export module Vitro.Graphics.ForwardRenderer;

import Vitro.Graphics.CommandList;
import Vitro.Graphics.Device;
import Vitro.Graphics.DeferredDeleter;
import Vitro.Graphics.FrameContext;
import Vitro.Graphics.SwapChain;
import Vitro.Trace.Log;

namespace vt
{
	export class ForwardRenderer
	{
	public:
		ForwardRenderer(Device& device) : device(device), cmd(device)
		{}

		void draw(SwapChain& swapChain)
		{
			auto& renderTarget = swapChain->acquireRenderTarget();
			cmd->begin();

			cmd->end();
		}

	private:
		Device&			  device;
		DeferredDeleter	  deferredDeleter;
		RenderCommandList cmd;
	};
}
