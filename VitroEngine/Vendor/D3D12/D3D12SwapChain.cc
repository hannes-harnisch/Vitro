module;
#include "Core/Macros.hh"
#include "D3D12API.hh"

#include <utility>
export module vt.D3D12.SwapChain;

import vt.App.Window;
import vt.Core.FixedList;
import vt.D3D12.Device;
import vt.D3D12.Handle;
import vt.Graphics.Device;
import vt.Graphics.Driver;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.SwapChainBase;

namespace vt::d3d12
{
	export class D3D12SwapChain final : public SwapChainBase
	{
	public:
		D3D12SwapChain(Driver const&	 driver,
					   Device&			 device,
					   RenderPass const& render_pass,
					   Window&			 window,
					   unsigned char	 buffer_count = DefaultBufferCount) :
			device(device.d3d12),
			buffer_count(buffer_count),
			tearing_supported(driver.d3d12.swap_chain_tearing_supported()),
			present_flags(tearing_supported ? DXGI_PRESENT_ALLOW_TEARING : 0),
			format(render_pass.d3d12.get_render_target_format(0))
		{
			initialize_swap_chain(driver.d3d12.get_factory(), window);
			initialize_render_target_heap();
			recreate_render_targets();
		}

		RenderTarget const& acquire_render_target() override
		{
			return render_targets[swap_chain->GetCurrentBackBufferIndex()];
		}

		void present() override
		{
			auto result = swap_chain->Present(is_vsync_enabled, present_flags);
			VT_ASSERT_RESULT(result, "Failed to present with D3D12 swap chain.");
		}

		void resize(WindowSizeEvent const& event) override
		{
			if(event.size.zero())
				return;

			device.flush_render_queue();
			render_targets.clear();
			unsigned flags = tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

			auto result = swap_chain->ResizeBuffers(buffer_count, event.size.width, event.size.height, format, flags);
			VT_ASSERT_RESULT(result, "Failed to resize D3D12 swap chain.");

			recreate_render_targets();
		}

		void enable_vsync() override
		{
			is_vsync_enabled = true;
			present_flags	 = 0;
		}

		void disable_vsync() override
		{
			is_vsync_enabled = false;
			if(tearing_supported)
				present_flags = DXGI_PRESENT_ALLOW_TEARING;
		}

	private:
		D3D12Device&						device;
		unsigned char						buffer_count;
		bool								tearing_supported;
		UINT								present_flags;
		DXGI_FORMAT							format;
		ComUnique<IDXGISwapChain3>			swap_chain;
		ComUnique<ID3D12DescriptorHeap>		render_target_heap;
		FixedList<RenderTarget, MaxBuffers> render_targets;

		void initialize_swap_chain(IDXGIFactory5* factory, Window& window)
		{
			HWND hwnd = window.native_handle();

			DXGI_SWAP_CHAIN_DESC1 const desc {
				.Width		 = 0,
				.Height		 = 0,
				.Format		 = format,
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
			auto result = factory->CreateSwapChainForHwnd(device.get_render_queue_ptr(), hwnd, &desc, nullptr, nullptr,
														  &raw_swap_chain_prototype);
			ComUnique<IDXGISwapChain1> swap_chain_prototype(raw_swap_chain_prototype);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 proxy swap chain.");

			IDXGISwapChain3* raw_swap_chain;
			result = swap_chain_prototype->QueryInterface(&raw_swap_chain);
			swap_chain.reset(raw_swap_chain);
			VT_ENSURE_RESULT(result, "Failed to query for D3D12 main swap chain.");

			result = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			VT_ENSURE_RESULT(result, "Failed to disable DXGI auto-fullscreen.");
		}

		void initialize_render_target_heap()
		{
			D3D12_DESCRIPTOR_HEAP_DESC const desc {
				.Type			= D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
				.NumDescriptors = buffer_count,
			};
			ID3D12DescriptorHeap* raw_heap;

			auto result = device.ptr()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&raw_heap));
			render_target_heap.reset(raw_heap);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 render target descriptor heap.");
		}

		void recreate_render_targets()
		{
			UINT size	= device.ptr()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			auto handle = render_target_heap->GetCPUDescriptorHandleForHeapStart();
			for(unsigned i = 0; i != buffer_count; ++i)
			{
				ID3D12Resource* render_target_ptr;

				auto result = swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_target_ptr));
				VT_ENSURE_RESULT(result, "Failed to get D3D12 swap chain buffer.");
				ComUnique<ID3D12Resource> render_target(render_target_ptr);

				device.ptr()->CreateRenderTargetView(render_target.get(), nullptr, handle);
				render_targets.emplace_back(std::move(render_target), handle);

				handle.ptr += size;
			}
		}
	};
}
