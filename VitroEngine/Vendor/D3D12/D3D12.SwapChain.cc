module;
#include "D3D12.API.hh"
#include "Trace/Assert.hh"
export module Vitro.D3D12.SwapChain;

import Vitro.Core.Array;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Driver;
import Vitro.Graphics.Handle;
import Vitro.Graphics.SwapChainBase;

namespace vt::d3d12
{
	export class SwapChain final : public SwapChainBase
	{
	public:
		SwapChain(DeviceHandle device, QueueHandle queue, void* nativeWindow, unsigned bufferCount = DefaultBufferCount) :
			bufferCount(bufferCount),
			presentFlags(vt::Driver::get().d3d12.swapChainTearingAvailable() ? DXGI_PRESENT_ALLOW_TEARING : 0),
			swapChain(makeSwapChain(queue.d3d12(), nativeWindow)),
			renderTargetHeap(makeRenderTargetHeap(device.d3d12()))
		{
			initializeRenderTargets(device.d3d12());
		}

		unsigned getNextRenderTargetIndex() override
		{
			return swapChain->GetCurrentBackBufferIndex();
		}

		void present() override
		{
			auto result = swapChain->Present(isVSyncEnabled, presentFlags);
			vtAssertResult(result, "Failed to present with D3D12 swap chain.");
		}

		void resize() override
		{
			auto result = swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			vtAssertResult(result, "Failed to resize D3D12 swap chain.");
		}

		void enableVSync() override
		{
			isVSyncEnabled = true;
			presentFlags   = 0;
		}

		void disableVSync() override
		{
			isVSyncEnabled = false;
			if(vt::Driver::get().d3d12.swapChainTearingAvailable())
				presentFlags = DXGI_PRESENT_ALLOW_TEARING;
		}

	private:
		unsigned							  bufferCount;
		UINT								  presentFlags;
		UniqueInterface<IDXGISwapChain3>	  swapChain;
		UniqueInterface<ID3D12DescriptorHeap> renderTargetHeap;
		UniqueInterface<ID3D12Resource>		  renderTargets[MaxBufferCount];

		IDXGISwapChain3* makeSwapChain(ID3D12CommandQueue* queue, void* nativeWindow)
		{
			UINT flags = 0;
			if(vt::Driver::get().d3d12.swapChainTearingAvailable())
				flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

			auto factory = vt::Driver::get().d3d12.handle();
			HWND hwnd	 = static_cast<HWND>(nativeWindow);

			DXGI_SWAP_CHAIN_DESC1 const desc {
				.Format		 = DXGI_FORMAT_R8G8B8A8_UNORM,
				.SampleDesc	 = {1, 0},
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = bufferCount,
				.SwapEffect	 = DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.Flags		 = flags,
			};
			IDXGISwapChain1* swapChainPtr;

			auto result = factory->CreateSwapChainForHwnd(queue, hwnd, &desc, nullptr, nullptr, &swapChainPtr);
			vtEnsureResult(result, "Failed to create D3D12 proxy swap chain.");
			UniqueInterface<IDXGISwapChain1> proxySwapChain(swapChainPtr);

			IDXGISwapChain3* mainSwapChain;
			result = proxySwapChain->QueryInterface(&mainSwapChain);
			vtEnsureResult(result, "Failed to query for D3D12 main swap chain.");

			result = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			vtEnsureResult(result, "Failed to disable DXGI auto-fullscreen.");
			return mainSwapChain;
		}

		ID3D12DescriptorHeap* makeRenderTargetHeap(ID3D12Device* device)
		{
			D3D12_DESCRIPTOR_HEAP_DESC const desc {
				.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				.NumDescriptors = bufferCount,
			};
			ID3D12DescriptorHeap* heap;

			auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
			vtEnsureResult(result, "Failed to create D3D12 render target descriptor heap.");
			return heap;
		}

		void initializeRenderTargets(ID3D12Device* device)
		{
			UINT size	= device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			auto handle = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
			for(unsigned i = 0; i < bufferCount; ++i)
			{
				ID3D12Resource* renderTargetPtr;

				auto result = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargetPtr));
				vtEnsureResult(result, "Failed to get D3D12 swap chain buffer.");
				UniqueInterface<ID3D12Resource> renderTarget(renderTargetPtr);

				device->CreateRenderTargetView(renderTarget.get(), nullptr, handle);
				renderTargets[i] = std::move(renderTarget);

				handle.ptr += size;
			}
		}
	};
}
