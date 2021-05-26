module;
#include "Direct3D.API.hh"
export module Vitro.Direct3D.SwapChain;

import Vitro.Direct3D.Unique;

namespace Direct3D
{
	export class SwapChain
	{
	private:
		Unique<IDXGISwapChain1> swapChain;
	};
}
