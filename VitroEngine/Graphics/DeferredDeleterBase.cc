export module Vitro.Graphics.DeferredDeleterBase;

import Vitro.Graphics.Handle;

namespace vt
{
	export class DeferredDeleterBase
	{
	public:
		virtual void submit(BufferHandle buffer)			   = 0;
		virtual void submit(TextureHandle texture)			   = 0;
		virtual void submit(PipelineHandle pipeline)		   = 0;
		virtual void submit(RenderPassHandle renderPass)	   = 0;
		virtual void submit(RootSignatureHandle rootSignature) = 0;
		virtual void deleteAll()							   = 0;
	};
}
