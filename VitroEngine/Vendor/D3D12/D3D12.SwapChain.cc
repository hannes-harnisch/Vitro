module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <cstdint>
export module Vitro.D3D12.SwapChain;

import Vitro.D3D12.Unique;
import Vitro.Graphics.SwapChainBase;
import Vitro.Math.Rectangle;

namespace D3D12
{
	export class SwapChain final : public SwapChainBase
	{
	public:
		SwapChain(void* const nativeWindow, uint32_t const bufferCount)
		{
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc {};
			swapChainDesc.Format	  = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.SampleDesc  = {1, 0};
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = bufferCount;
			swapChainDesc.Scaling	  = DXGI_SCALING_STRETCH;
			swapChainDesc.SwapEffect  = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swapChainDesc.AlphaMode	  = DXGI_ALPHA_MODE_UNSPECIFIED;
			swapChainDesc.Flags		  = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		}

		void present() override
		{
			swapChain->Present(0, 0);
		}

		void resize() override
		{
			swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		}

	private:
		Unique<IDXGISwapChain> swapChain;
	};
}
