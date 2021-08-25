module;
#include "Core/Macros.hh"
#include "D3D12.API.hh"

#include <utility>
export module vt.D3D12.SwapChain;

import vt.Core.FixedList;
import vt.D3D12.Utils;
import vt.Graphics.Device;
import vt.Graphics.Driver;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.SwapChainBase;

namespace vt::d3d12
{
	export class SwapChain final : public SwapChainBase
	{
	public:
		SwapChain(vt::Driver const&		driver,
				  vt::Device const&		dev,
				  vt::RenderPass const& render_pass,
				  void*					native_window,
				  unsigned				buffer_count = DefaultBufferCount) :
			tearing_supported(driver.d3d12.swap_chain_tearing_supported()),
			present_flags(tearing_supported ? DXGI_PRESENT_ALLOW_TEARING : 0),
			swap_chain(
				make_swap_chain(driver.d3d12.get(), dev.d3d12.render_queue_handle(), render_pass, native_window, buffer_count)),
			render_target_heap(make_render_target_heap(dev.d3d12.get(), buffer_count))
		{
			auto device = dev.d3d12.get();
			UINT size	= device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			auto handle = render_target_heap->GetCPUDescriptorHandleForHeapStart();
			for(unsigned i = 0; i != buffer_count; ++i)
			{
				ID3D12Resource* render_target_ptr;

				auto result = swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_target_ptr));
				VT_ENSURE_RESULT(result, "Failed to get D3D12 swap chain buffer.");
				ComUnique<ID3D12Resource> render_target(render_target_ptr);

				device->CreateRenderTargetView(render_target.get(), nullptr, handle);
				render_targets.emplace_back(std::move(render_target), handle);

				handle.ptr += size;
			}
		}

		vt::RenderTarget& acquire_render_target() override
		{
			return render_targets[swap_chain->GetCurrentBackBufferIndex()];
		}

		void present() override
		{
			auto result = swap_chain->Present(is_v_sync_enabled, present_flags);
			VT_ASSERT_RESULT(result, "Failed to present with D3D12 swap chain.");
		}

		void resize() override
		{
			auto result = swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			VT_ASSERT_RESULT(result, "Failed to resize D3D12 swap chain.");
		}

		void enable_v_sync() override
		{
			is_v_sync_enabled = true;
			present_flags	  = 0;
		}

		void disable_v_sync() override
		{
			is_v_sync_enabled = false;
			if(tearing_supported)
				present_flags = DXGI_PRESENT_ALLOW_TEARING;
		}

	private:
		bool									tearing_supported;
		UINT									present_flags;
		ComUnique<IDXGISwapChain3>				swap_chain;
		ComUnique<ID3D12DescriptorHeap>			render_target_heap;
		FixedList<vt::RenderTarget, MaxBuffers> render_targets;

		decltype(swap_chain) make_swap_chain(IDXGIFactory5*		   factory,
											 ID3D12CommandQueue*   queue,
											 vt::RenderPass const& render_pass,
											 void*				   native_window,
											 unsigned			   buffer_count)
		{
			HWND hwnd = static_cast<HWND>(native_window);

			DXGI_SWAP_CHAIN_DESC1 const desc {
				.Format		 = render_pass.d3d12.attachments[0].format,
				.Stereo		 = false,
				.SampleDesc	 = {1, 0},
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = buffer_count,
				.Scaling	 = DXGI_SCALING_STRETCH,
				.SwapEffect	 = DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.AlphaMode	 = DXGI_ALPHA_MODE_UNSPECIFIED,
				.Flags		 = tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u,
			};
			IDXGISwapChain1* raw_swap_chain_prototype;
			auto result = factory->CreateSwapChainForHwnd(queue, hwnd, &desc, nullptr, nullptr, &raw_swap_chain_prototype);
			ComUnique<IDXGISwapChain1> swap_chain_prototype(raw_swap_chain_prototype);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 proxy swap chain.");

			IDXGISwapChain3* raw_swap_chain;
			result = swap_chain_prototype->QueryInterface(&raw_swap_chain);
			decltype(swap_chain) fresh_swap_chain(raw_swap_chain);
			VT_ENSURE_RESULT(result, "Failed to query for D3D12 main swap chain.");

			result = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			VT_ENSURE_RESULT(result, "Failed to disable DXGI auto-fullscreen.");
			return fresh_swap_chain;
		}

		decltype(render_target_heap) make_render_target_heap(ID3D12Device4* device, unsigned buffer_count)
		{
			D3D12_DESCRIPTOR_HEAP_DESC const desc {
				.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				.NumDescriptors = buffer_count,
			};
			ID3D12DescriptorHeap* raw_heap;

			auto result = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&raw_heap));

			decltype(render_target_heap) fresh_heap(raw_heap);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 render target descriptor heap.");

			return fresh_heap;
		}
	};
}
