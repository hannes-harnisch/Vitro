export module Vitro.Graphics.Resource;

import Vitro.Graphics.DeferredUnique;
import Vitro.Graphics.Device;
import Vitro.Graphics.Handle;
import Vitro.Graphics.PipelineDescription;

namespace vt
{
	export class Pipeline
	{
	public:
		Pipeline(Device& device, RenderPipelineDescription const& desc) : pipeline(device->makeRenderPipeline(desc))
		{}

	private:
		DeferredUnique<PipelineHandle> pipeline;
	};
}
