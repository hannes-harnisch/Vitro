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
		Pipeline(Device& device, PipelineState const& state) : pipeline(device->makePipeline(state))
		{}

	private:
		DeferredUnique<PipelineHandle> pipeline;
	};
}
