export module Vitro.Graphics.SwapChainBase;

export class SwapChainBase
{
public:
	virtual void present() = 0;
	virtual void resize()  = 0;
};
