export module Vitro.Graphics.CommandListBase;

import Vitro.Graphics.Handle;
import Vitro.Math.Rectangle;

namespace vt
{
	export enum class QueuePurpose {
		Copy,
		Compute,
		Graphics,
	};

	export class CopyCommandListBase
	{
	public:
		virtual void begin() = 0;
		virtual void end()	 = 0;
	};

	export class ComputeCommandListBase : public CopyCommandListBase
	{
	public:
		virtual void bindPipeline(PipelineHandle pipeline) = 0;
	};

	export class GraphicsCommandListBase : public ComputeCommandListBase
	{
	public:
		virtual void bindViewport(Rectangle viewport) = 0;
		virtual void bindScissor(Rectangle scissor)	  = 0;
		virtual void transitionToNextSubpass()		  = 0;
		virtual void endRenderPass()				  = 0;
	};
}
