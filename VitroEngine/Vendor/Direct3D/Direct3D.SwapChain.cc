module;
#include "Direct3D.API.hh"
#include "Trace/Assert.hh"

#include <cstdint>
export module Vitro.Direct3D.SwapChain;

import Vitro.Direct3D.Unique;
import Vitro.Math.Rectangle;

namespace Direct3D
{
	export class SwapChain
	{
	public:
		// TODO ICE: Rectangle here
		SwapChain(void* const nativeWindow, uint32_t width, uint32_t height, const uint32_t bufferCount)
		{
			Unique<IDXGIFactory4> factory;
			UINT factoryFlags = 0;
#if VE_DEBUG
			factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
			veEnsureResult(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory)), "Failed to create DXGI factory.");

			DXGI_SWAP_CHAIN_DESC1 swapChainDesc {};
			swapChainDesc.Width		  = width;
			swapChainDesc.Height	  = height;
			swapChainDesc.Format	  = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.Stereo	  = FALSE;
			swapChainDesc.SampleDesc  = {1, 0};
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = bufferCount;
			swapChainDesc.Scaling	  = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode	  = DXGI_ALPHA_MODE_UNSPECIFIED;
			swapChainDesc.Flags		  = checkTearingSupport() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
		}

	private:
		Unique<IDXGISwapChain4> swapChain;

		static bool checkTearingSupport()
		{
			BOOL hasTearing {};

			// Rather than create the DXGI 1.5 factory interface directly, we create the
			// DXGI 1.4 interface and query for the 1.5 interface. This is to enable the
			// graphics debugging tools which will not support the 1.5 factory interface
			// until a future update.
			Unique<IDXGIFactory4> factory4;
			if(SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
			{
				Unique<IDXGIFactory5> factory5;
				if(SUCCEEDED(factory4.queryFor(&factory5)))
					if(FAILED(
						   factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &hasTearing, sizeof hasTearing)))
						hasTearing = FALSE;
			}
			return hasTearing == TRUE;
		}
	};
}
