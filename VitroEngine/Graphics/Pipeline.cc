export module Vitro.Graphics.Pipeline;

import Vitro.Core.Unique;
import Vitro.Graphics.Handle;

namespace vt
{
	export class Pipeline
	{
	public:
	private:
		Unique<PipelineHandle> handle;
	};
}
