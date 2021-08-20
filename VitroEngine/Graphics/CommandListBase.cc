export module Vitro.Graphics.CommandListBase;

import Vitro.Core.Rectangle;
import Vitro.Graphics.RenderPass;
import Vitro.Graphics.RenderTarget;
import Vitro.Graphics.Resource;
import Vitro.Graphics.RootSignature;

namespace vt
{
	export enum class CommandType {
		Copy,
		Compute,
		Render,
	};

	export enum class IndexFormat {
		UInt16,
		UInt32,
	};

	export class CopyCommandListBase
	{
	public:
		virtual ~CopyCommandListBase() = default;

		virtual void* handle() = 0;
		virtual void  reset()  = 0;
		virtual void  begin()  = 0;
		virtual void  end()	   = 0;
	};

	export class ComputeCommandListBase : public CopyCommandListBase
	{
	public:
		virtual void bindPipeline(Pipeline const& pipeline)							 = 0;
		virtual void bindRootSignature(RootSignature const& rootSignature)			 = 0;
		virtual void pushConstants(void const* data, unsigned size, unsigned offset) = 0;
		virtual void dispatch(unsigned xCount, unsigned yCount, unsigned zCount)	 = 0;
	};

	export class RenderCommandListBase : public ComputeCommandListBase
	{
	public:
		virtual void beginRenderPass(RenderPass const& renderPass, RenderTarget const& renderTarget)				  = 0;
		virtual void transitionToNextSubpass()																		  = 0;
		virtual void endRenderPass()																				  = 0;
		virtual void bindIndexBuffer(Buffer const& buffer, IndexFormat format)										  = 0;
		virtual void bindPrimitiveTopology(PrimitiveTopology topology)												  = 0;
		virtual void bindViewport(Viewport viewport)																  = 0;
		virtual void bindScissor(Rectangle scissor)																	  = 0;
		virtual void draw(unsigned vertexCount, unsigned instanceCount, unsigned firstVertex, unsigned firstInstance) = 0;
		virtual void drawIndexed(unsigned indexCount,
								 unsigned instanceCount,
								 unsigned firstIndex,
								 int	  vertexOffset,
								 unsigned firstInstance)															  = 0;
	};
}
