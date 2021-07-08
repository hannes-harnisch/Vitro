export module Vitro.Graphics.SwapChainBase;

namespace vt
{
	export class SwapChainBase
	{
	public:
		virtual unsigned getNextRenderTargetIndex() = 0;
		virtual void present()						= 0;
		virtual void resize()						= 0;
	};
}
