module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.SwapChain;

import Vitro.Core.Array;
import Vitro.D3D12.ComUnique;
import Vitro.Graphics.Device;
import Vitro.Graphics.Driver;
import Vitro.Graphics.SwapChainBase;

namespace vt::d3d12
{
	export class SwapChain final : public SwapChainBase
	{
	public:
		SwapChain(vt::Driver const& driver, vt::Device const& device, void* const nativeWindow, unsigned const bufferCount) :
			bufferCount(bufferCount),
			swapChain(makeSwapChain(driver, device.d3d12.getGraphicsQueue(), nativeWindow)),
			renderTargetHeap(makeRenderTargetHeap(device.d3d12.getDevice()))
		{
			initializeRenderTargets(device.d3d12.getDevice());
		}

		unsigned getNextRenderTargetIndex() const override
		{
			return swapChain->GetCurrentBackBufferIndex();
		}

		void present() override
		{
			auto result = swapChain->Present(0, 0);
			vtAssertResult(result, "Failed to present with D3D12 swap chain.");
		}

		void resize() override
		{
			auto result = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			vtAssertResult(result, "Failed to resize D3D12 swap chain.");
		}

	private:
		constexpr static unsigned MaxBufferCount = 3;

		unsigned bufferCount;
		ComUnique<IDXGISwapChain3> swapChain;
		ComUnique<ID3D12DescriptorHeap> renderTargetHeap;
		ComUnique<ID3D12Resource> renderTargets[MaxBufferCount];

		ComUnique<IDXGISwapChain3>
		makeSwapChain(vt::Driver const& driver, ID3D12CommandQueue* const queue, void* const nativeWindow)
		{
			UINT flags = 0;
			if(driver.d3d12.isSwapChainTearingAvailable())
				flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

			DXGI_SWAP_CHAIN_DESC1 const desc {
				.Format		 = DXGI_FORMAT_R8G8B8A8_UNORM,
				.SampleDesc	 = {1, 0},
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = bufferCount,
				.SwapEffect	 = DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.Flags		 = flags,
			};
			auto const factory = driver.d3d12.getFactory();
			HWND const hwnd	   = static_cast<HWND>(nativeWindow);
			ComUnique<IDXGISwapChain1> proxySwapChain;
			auto result = factory->CreateSwapChainForHwnd(queue, hwnd, &desc, nullptr, nullptr, &proxySwapChain);
			vtEnsureResult(result, "Failed to create D3D12 proxy swap chain.");

			ComUnique<IDXGISwapChain3> mainSwapChain;
			result = proxySwapChain.queryFor(&mainSwapChain);
			vtEnsureResult(result, "Failed to query for D3D12 main swap chain.");

			result = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			vtEnsureResult(result, "Failed to disable DXGI auto-fullscreen.");

			return mainSwapChain;
		}

		ComUnique<ID3D12DescriptorHeap> makeRenderTargetHeap(ID3D12Device* const device)
		{
			D3D12_DESCRIPTOR_HEAP_DESC const desc {
				.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				.NumDescriptors = bufferCount,
			};
			ComUnique<ID3D12DescriptorHeap> heap;
			auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
			vtEnsureResult(result, "Failed to create D3D12 render target descriptor heap.");
			return heap;
		}

		void initializeRenderTargets(ID3D12Device* const device)
		{
			UINT const size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			auto handle		= renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
			for(unsigned i = 0; i < bufferCount; ++i)
			{
				ComUnique<ID3D12Resource> backBuffer;
				auto result = swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer));
				vtEnsureResult(result, "Failed to get D3D12 swap chain buffer.");
				device->CreateRenderTargetView(backBuffer, nullptr, handle);
				renderTargets[i] = std::move(backBuffer);
				handle.ptr += size;
			}
		}
	};
}
