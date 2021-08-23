module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <utility>
export module Vitro.D3D12.SwapChain;

import Vitro.Core.FixedList;
import Vitro.D3D12.Utils;
import Vitro.Graphics.Device;
import Vitro.Graphics.Driver;
import Vitro.Graphics.RenderPass;
import Vitro.Graphics.RenderTarget;
import Vitro.Graphics.SwapChainBase;

namespace vt::d3d12
{
	export class SwapChain final : public SwapChainBase
	{
	public:
		SwapChain(vt::Driver const&		driver,
				  vt::Device const&		dev,
				  vt::RenderPass const& renderPass,
				  void*					nativeWindow,
				  unsigned				bufferCount = DefaultBufferCount) :
			tearingSupported(driver.d3d12.swapChainTearingSupported()),
			presentFlags(tearingSupported ? DXGI_PRESENT_ALLOW_TEARING : 0),
			swapChain(makeSwapChain(driver.d3d12.get(), dev.d3d12.renderQueueHandle(), renderPass, nativeWindow, bufferCount)),
			renderTargetHeap(makeRenderTargetHeap(dev.d3d12.get(), bufferCount))
		{
			auto device = dev.d3d12.get();
			UINT size	= device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			auto handle = renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
			for(unsigned i = 0; i != bufferCount; ++i)
			{
				ID3D12Resource* renderTargetPtr;

				auto result = swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargetPtr));
				vtEnsureResult(result, "Failed to get D3D12 swap chain buffer.");
				ComUnique<ID3D12Resource> renderTarget(renderTargetPtr);

				device->CreateRenderTargetView(renderTarget.get(), nullptr, handle);
				renderTargets.emplace_back(std::move(renderTarget), handle);

				handle.ptr += size;
			}
		}

		vt::RenderTarget& acquireRenderTarget() override
		{
			return renderTargets[swapChain->GetCurrentBackBufferIndex()];
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
			if(tearingSupported)
				presentFlags = DXGI_PRESENT_ALLOW_TEARING;
		}

	private:
		bool									tearingSupported;
		UINT									presentFlags;
		ComUnique<IDXGISwapChain3>				swapChain;
		ComUnique<ID3D12DescriptorHeap>			renderTargetHeap;
		FixedList<vt::RenderTarget, MaxBuffers> renderTargets;

		decltype(swapChain) makeSwapChain(IDXGIFactory5*		factory,
										  ID3D12CommandQueue*	queue,
										  vt::RenderPass const& renderPass,
										  void*					nativeWindow,
										  unsigned				bufferCount)
		{
			HWND hwnd = static_cast<HWND>(nativeWindow);

			DXGI_SWAP_CHAIN_DESC1 const desc {
				.Format		 = renderPass.d3d12.attachments[0].format,
				.Stereo		 = false,
				.SampleDesc	 = {1, 0},
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = bufferCount,
				.Scaling	 = DXGI_SCALING_STRETCH,
				.SwapEffect	 = DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.AlphaMode	 = DXGI_ALPHA_MODE_UNSPECIFIED,
				.Flags		 = tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u,
			};
			IDXGISwapChain1* rawSwapChainPrototype;
			auto result = factory->CreateSwapChainForHwnd(queue, hwnd, &desc, nullptr, nullptr, &rawSwapChainPrototype);
			ComUnique<IDXGISwapChain1> swapChainPrototype(rawSwapChainPrototype);
			vtEnsureResult(result, "Failed to create D3D12 proxy swap chain.");

			IDXGISwapChain3* rawSwapChain;
			result = swapChainPrototype->QueryInterface(&rawSwapChain);
			decltype(swapChain) freshSwapChain(rawSwapChain);
			vtEnsureResult(result, "Failed to query for D3D12 main swap chain.");

			result = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			vtEnsureResult(result, "Failed to disable DXGI auto-fullscreen.");
			return freshSwapChain;
		}

		decltype(renderTargetHeap) makeRenderTargetHeap(ID3D12Device4* device, unsigned bufferCount)
		{
			D3D12_DESCRIPTOR_HEAP_DESC const desc {
				.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				.NumDescriptors = bufferCount,
			};
			ID3D12DescriptorHeap* rawHeap;

			auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rawHeap));

			decltype(renderTargetHeap) freshHeap(rawHeap);
			vtEnsureResult(result, "Failed to create D3D12 render target descriptor heap.");

			return freshHeap;
		}
	};
}
