export module Vitro.Graphics.ForwardRenderer;

import Vitro.Graphics.CommandList;
import Vitro.Graphics.Device;
import Vitro.Graphics.DeletionQueue;

namespace vt
{
	export class ForwardRenderer
	{
	public:
		ForwardRenderer(Device& device) : commandList(device)
		{}

	private:
		DeletionQueue deletionQueue;
		RenderCommandList commandList;
	};
}
