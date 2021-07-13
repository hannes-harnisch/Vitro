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
		virtual void bindPipeline(PipelineHandle)			= 0;
		virtual void bindRootSignature(RootSignatureHandle) = 0;
	};

	export class RenderCommandListBase : public ComputeCommandListBase
	{
	public:
		virtual void bindViewport(Rectangle)							   = 0;
		virtual void bindScissor(Rectangle)								   = 0;
		virtual void beginRenderPass(RenderPassHandle, RenderTargetHandle) = 0;
		virtual void transitionToNextSubpass()							   = 0;
		virtual void endRenderPass()									   = 0;
	};
}
