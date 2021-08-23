module;
#include <span>
export module Vitro.Graphics.CommandListBase;

import Vitro.Core.FixedList;
import Vitro.Core.Rectangle;
import Vitro.Core.Vector;
import Vitro.Graphics.Driver;
import Vitro.Graphics.RenderPass;
import Vitro.Graphics.RenderPassInfo;
import Vitro.Graphics.RenderTarget;
import Vitro.Graphics.Resource;
import Vitro.Graphics.RootSignature;

namespace vt
{
	export enum CommandType : unsigned char {
		Copy,
		Compute,
		Render,
	};

	export union ClearValue
	{
		Float4 color = {};
		struct
		{
			float		  depth;
			unsigned char stencil;
		};
	};

	export class CopyCommandListBase
	{
	public:
		virtual ~CopyCommandListBase() = default;

		virtual CommandListHandle handle() = 0;
		virtual void			  reset()  = 0;
		virtual void			  begin()  = 0;
		virtual void			  end()	   = 0;
	};

	export class ComputeCommandListBase : public CopyCommandListBase
	{
	public:
		virtual void bindComputePipeline(Pipeline const& pipeline)							= 0;
		virtual void bindComputeRootSignature(RootSignature const& rootSignature)			= 0;
		virtual void pushComputeConstants(void const* data, unsigned size, unsigned offset) = 0;
		virtual void dispatch(unsigned xCount, unsigned yCount, unsigned zCount)			= 0;
	};

	export class RenderCommandListBase : public ComputeCommandListBase
	{
	public:
		virtual void bindRenderPipeline(Pipeline const& pipeline)													  = 0;
		virtual void bindRenderRootSignature(RootSignature const& rootSignature)									  = 0;
		virtual void pushRenderConstants(void const* data, unsigned size, unsigned offset)							  = 0;
		virtual void beginRenderPass(RenderPass const&			 renderPass,
									 RenderTarget const&		 renderTarget,
									 std::span<ClearValue const> clearValues = {})									  = 0;
		virtual void transitionToNextSubpass()																		  = 0;
		virtual void endRenderPass()																				  = 0;
		virtual void bindVertexBuffers(unsigned					 firstBuffer,
									   std::span<Buffer const>	 buffers,
									   std::span<unsigned const> byteOffsets)										  = 0;
		virtual void bindIndexBuffer(Buffer const& buffer, unsigned byteOffset)										  = 0;
		virtual void bindPrimitiveTopology(PrimitiveTopology topology)												  = 0;
		virtual void bindViewports(std::span<Viewport const> viewports)												  = 0;
		virtual void bindScissors(std::span<Rectangle const> scissors)												  = 0;
		virtual void draw(unsigned vertexCount, unsigned instanceCount, unsigned firstVertex, unsigned firstInstance) = 0;
		virtual void drawIndexed(unsigned indexCount,
								 unsigned instanceCount,
								 unsigned firstIndex,
								 int	  vertexOffset,
								 unsigned firstInstance)															  = 0;
	};
}
