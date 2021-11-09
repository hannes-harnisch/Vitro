module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"

#include <optional>
#include <utility>
export module vt.Graphics.D3D12.SwapChain;

import vt.App.Window;
import vt.Core.FixedList;
import vt.Graphics.SwapChainBase;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Queue;

namespace vt::d3d12
{
	export class D3D12SwapChain final : public SwapChainBase
	{
	public:
		D3D12SwapChain(Queue& render_queue, IDXGIFactory5& factory, Window& window, uint8_t buffer_count) :
			buffer_count(buffer_count),
			tearing_supported(check_tearing_support(factory)),
			present_flags(tearing_supported ? DXGI_PRESENT_ALLOW_TEARING : 0),
			buffer_size(window.get_size()),
			render_queue_fence(render_queue.get_fence()),
			fence_values(buffer_count, 0)
		{
			HWND hwnd = window.native_handle();

			DXGI_SWAP_CHAIN_DESC1 const desc {
				.Width	= 0, // Zero means the D3D12 runtime deduces the dimensions from the window handle.
				.Height = 0,
				.Format = PREFERRED_FORMAT,
				.Stereo = false,
				.SampleDesc {
					.Count	 = 1,
					.Quality = 0,
				},
				.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
				.BufferCount = buffer_count,
				.Scaling	 = DXGI_SCALING_STRETCH,
				.SwapEffect	 = DXGI_SWAP_EFFECT_FLIP_DISCARD,
				.AlphaMode	 = DXGI_ALPHA_MODE_IGNORE,
				.Flags		 = tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u,
			};
			ComUnique<IDXGISwapChain1> swap_chain_prototype;

			auto result = factory.CreateSwapChainForHwnd(render_queue.get_handle(), hwnd, &desc, nullptr, nullptr,
														 std::out_ptr(swap_chain_prototype));
			VT_CHECK_RESULT(result, "Failed to create D3D12 proxy swap chain.");

			result = swap_chain_prototype->QueryInterface(VT_COM_OUT(swap_chain));
			VT_CHECK_RESULT(result, "Failed to query for D3D12 main swap chain.");

			result = factory.MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			VT_CHECK_RESULT(result, "Failed to disable DXGI auto-fullscreen.");

			acquire_back_buffers();
		}

		ImageFormat get_format() const override
		{
			return ImageFormat::Rgba8UNorm; // This format is always supported in D3D12 and will be preferred.
		}

		unsigned get_current_image_index() const override
		{
			return swap_chain->GetCurrentBackBufferIndex();
		}

		unsigned count_presents() const override
		{
			UINT count;
			auto result = swap_chain->GetLastPresentCount(&count);
			VT_CHECK_RESULT(result, "Failed to query D3D12 swap chain present count.");
			return count;
		}

		unsigned count_buffers() const override
		{
			return buffer_count;
		}

		Extent get_size() const override
		{
			return buffer_size;
		}

		void resize(Extent size) override
		{
			back_buffers.clear();

			buffer_size		= size;
			unsigned flags	= tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
			auto	 result = swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, flags);
			VT_CHECK_RESULT(result, "Failed to resize D3D12 swap chain.");

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

		std::optional<SyncToken> request_frame() override
		{
			return SyncToken {
				render_queue_fence,
				fence_values[get_current_image_index()],
			};
		}

		void present(Queue& render_queue)
		{
			unsigned index = swap_chain->GetCurrentBackBufferIndex();

			auto result = swap_chain->Present(is_vsync_enabled, present_flags);
			VT_CHECK_RESULT(result, "Failed to present with D3D12 swap chain.");

			fence_values[index] = render_queue.signal();
		}

		ID3D12Resource* get_back_buffer_ptr(size_t index) const
		{
			return back_buffers[index].get();
		}

	private:
		static constexpr DXGI_FORMAT PREFERRED_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;

		uint8_t					   buffer_count;
		bool					   tearing_supported;
		UINT					   present_flags;
		Extent					   buffer_size;
		ID3D12Fence*			   render_queue_fence;
		ComUnique<IDXGISwapChain3> swap_chain;

		FixedList<ComUnique<ID3D12Resource>, MAX_BUFFERS> back_buffers;
		FixedList<uint64_t, MAX_BUFFERS>				  fence_values;

		static bool check_tearing_support(IDXGIFactory5& factory)
		{
			BOOL supported;
			auto result = factory.CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &supported, sizeof supported);
			VT_CHECK_RESULT(result, "Failed to check for swap chain tearing support.");
			return supported;
		}

		void acquire_back_buffers()
		{
			for(int i = 0; i != buffer_count; ++i)
			{
				ComUnique<ID3D12Resource> back_buffer;

				auto result = swap_chain->GetBuffer(i, VT_COM_OUT(back_buffer));
				VT_CHECK_RESULT(result, "Failed to get D3D12 swap chain buffer.");

				back_buffers.emplace_back(std::move(back_buffer));
			}
		}
	};
}
