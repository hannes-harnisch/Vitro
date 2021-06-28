module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"

#include <cstdint>
export module Vitro.D3D12.SwapChain;

import Vitro.D3D12.Unique;
import Vitro.Graphics.Device;
import Vitro.Graphics.Driver;
import Vitro.Graphics.SwapChainBase;

namespace D3D12
{
	export class SwapChain final : public SwapChainBase
	{
	public:
		SwapChain(::Driver const& driver, ::Device const& device, void* const windowHandle, uint32_t const bufferCount) :
			swapChain(makeSwapChain(driver, device, windowHandle, bufferCount))
		{}

		void present() override
		{
			swapChain->Present(0, 0);
		}

		void resize() override
		{
			swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		}

	private:
		Unique<IDXGISwapChain1> swapChain;

		static Unique<IDXGISwapChain1>
		makeSwapChain(::Driver const& driver, ::Device const& device, void* const windowHandle, uint32_t const bufferCount)
		{
			UINT flags = 0;
			if(driver.d3d12().isSwapChainTearingAvailable())
				flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

			DXGI_SWAP_CHAIN_DESC1 desc {.Format		 = DXGI_FORMAT_R8G8B8A8_UNORM,
										.SampleDesc	 = {1, 0},
										.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
										.BufferCount = bufferCount,
										.SwapEffect	 = DXGI_SWAP_EFFECT_FLIP_DISCARD,
										.AlphaMode	 = DXGI_ALPHA_MODE_UNSPECIFIED,
										.Flags		 = flags};

			auto const factory = driver.d3d12().getFactory();
			auto const queue   = device.d3d12().getGraphicsQueue();
			HWND const hwnd	   = static_cast<HWND>(windowHandle);
			Unique<IDXGISwapChain1> swapChain;
			auto result = factory->CreateSwapChainForHwnd(queue, hwnd, &desc, nullptr, nullptr, &swapChain);
			vtEnsureResult(result, "Failed to create D3D12 swap chain.");

			result = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			vtEnsureResult(result, "Failed to disable DXGI auto-fullscreen.");

			return swapChain;
		}
	};
}
