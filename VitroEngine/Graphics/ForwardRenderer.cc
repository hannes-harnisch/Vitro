module;
#include <new>
export module Vitro.Graphics.ForwardRenderer;

import Vitro.Graphics.CommandList;
import Vitro.Graphics.Device;
import Vitro.Graphics.DeferredDeleter;

namespace vt
{
	export class ForwardRenderer
	{
	public:
		ForwardRenderer(Device& device) : commandList(device)
		{}

	private:
		DeferredDeleter deletionQueue;
		RenderCommandList commandList;
	};
}
