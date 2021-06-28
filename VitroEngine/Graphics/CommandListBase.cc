export module Vitro.Graphics.CommandListBase;

import Vitro.Graphics.Handle;
import Vitro.Math.Rectangle;

namespace vt
{
	export class CommandListBase
	{
	public:
		virtual void begin()							   = 0;
		virtual void bindPipeline(PipelineHandle pipeline) = 0;
		virtual void bindViewport(Rectangle viewport)	   = 0;
		virtual void bindScissor(Rectangle scissor)		   = 0;
		virtual void endRenderPass()					   = 0;
		virtual void end()								   = 0;
	};
}
