module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <utility>
export module vt.Graphics.D3D12.SwapChain;

import vt.App.Window;
import vt.Core.FixedList;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Queue;
import vt.Graphics.Driver;
import vt.Graphics.SwapChainBase;

namespace vt::d3d12
{
	export class D3D12SwapChain final : public SwapChainBase
	{
	public:
		D3D12SwapChain(Queue& in_render_queue, Driver& driver, Window& window, uint8_t buffer_count) :
			render_queue(&in_render_queue),
			buffer_count(buffer_count),
			tearing_supported(driver.d3d12.swap_chain_tearing_supported()),
			present_flags(tearing_supported ? DXGI_PRESENT_ALLOW_TEARING : 0)
		{
			auto factory = driver.d3d12.get_factory();
			HWND hwnd	 = window.native_handle();

			DXGI_SWAP_CHAIN_DESC1 const desc {
				.Width		 = 0,
				.Height		 = 0,
				.Format		 = DESIRED_FORMAT,
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
			auto			 result = factory->CreateSwapChainForHwnd(render_queue->ptr(), hwnd, &desc, nullptr, nullptr,
														  &raw_swap_chain_prototype);
			ComUnique<IDXGISwapChain1> swap_chain_prototype(raw_swap_chain_prototype);
			VT_ENSURE_RESULT(result, "Failed to create D3D12 proxy swap chain.");

			IDXGISwapChain3* raw_swap_chain;
			result = swap_chain_prototype->QueryInterface(&raw_swap_chain);
			swap_chain.reset(raw_swap_chain);
			VT_ENSURE_RESULT(result, "Failed to query for D3D12 main swap chain.");

			result = factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			VT_ENSURE_RESULT(result, "Failed to disable DXGI auto-fullscreen.");

			acquire_back_buffers();
		}

		unsigned get_current_image_index() const override
		{
			return swap_chain->GetCurrentBackBufferIndex();
		}

		unsigned get_buffer_count() const override
		{
			return buffer_count;
		}

		void present() override
		{
			auto result = swap_chain->Present(is_vsync_enabled, present_flags);
			VT_ASSERT_RESULT(result, "Failed to present with D3D12 swap chain.");
		}

		void resize(WindowSizeEvent const& event) override
		{
			render_queue->wait_for_idle();
			back_buffers.clear();

			unsigned flags = tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
			auto result = swap_chain->ResizeBuffers(buffer_count, event.size.width, event.size.height, DESIRED_FORMAT, flags);
			VT_ASSERT_RESULT(result, "Failed to resize D3D12 swap chain.");

			acquire_back_buffers();
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

		ID3D12Resource* get_back_buffer_ptr(size_t index) const
		{
			return back_buffers[index].get();
		}

	private:
		static constexpr DXGI_FORMAT DESIRED_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

		Queue*											  render_queue;
		uint8_t											  buffer_count;
		bool											  tearing_supported;
		UINT											  present_flags;
		ComUnique<IDXGISwapChain3>						  swap_chain;
		FixedList<ComUnique<ID3D12Resource>, MAX_BUFFERS> back_buffers;

		void acquire_back_buffers()
		{
			for(int i = 0; i != buffer_count; ++i)
			{
				ID3D12Resource* render_target_ptr;

				auto result = swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_target_ptr));
				VT_ENSURE_RESULT(result, "Failed to get D3D12 swap chain buffer.");

				back_buffers.emplace_back(render_target_ptr);
			}
		}
	};
}
