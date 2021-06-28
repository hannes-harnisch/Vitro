export module Vitro.Graphics.SwapChainBase;

namespace vt
{
	export class SwapChainBase
	{
	public:
		virtual void present() = 0;
		virtual void resize()  = 0;
	};
}
