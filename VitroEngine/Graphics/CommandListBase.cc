export module Vitro.Graphics.CommandListBase;

import Vitro.Graphics.Handle;
import Vitro.Math.Rectangle;

namespace vt
{
	export enum class QueuePurpose {
		Copy,
		Compute,
		Render,
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
		virtual void bindPipeline(PipelineHandle pipeline)						 = 0;
		virtual void bindRootSignature(RootSignatureHandle rootSignature)		 = 0;
		virtual void dispatch(unsigned xCount, unsigned yCount, unsigned zCount) = 0;
	};

	export class RenderCommandListBase : public ComputeCommandListBase
	{
	public:
		virtual void beginRenderPass(RenderPassHandle renderPass, RenderTargetHandle renderTarget)					  = 0;
		virtual void transitionToNextSubpass()																		  = 0;
		virtual void endRenderPass()																				  = 0;
		virtual void bindViewport(Rectangle viewport)																  = 0;
		virtual void bindScissor(Rectangle scissor)																	  = 0;
		virtual void draw(unsigned vertexCount, unsigned instanceCount, unsigned firstVertex, unsigned firstInstance) = 0;
		virtual void drawIndexed(unsigned indexCount,
								 unsigned instanceCount,
								 unsigned firstIndex,
								 int vertexOffset,
								 unsigned firstInstance)															  = 0;
	};
}
