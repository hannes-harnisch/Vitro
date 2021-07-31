export module Vitro.Graphics.Resource;

import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.Device;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineState;

namespace vt
{
	export class Pipeline
	{
	public:
		Pipeline(Device& device, RenderPipelineState const& state) : pipeline(device->makeRenderPipeline(state))
		{}

	private:
		DeferredUnique<PipelineHandle> pipeline;
	};
}
