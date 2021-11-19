module;
#include "D3D12API.hpp"

#include <optional>
export module vt.Graphics.D3D12.SwapChain;

import vt.App.Window;
import vt.Core.FixedList;
import vt.Graphics.AbstractSwapChain;
import vt.Graphics.D3D12.Handle;
import vt.Graphics.D3D12.Queue;

namespace vt::d3d12
{
	export class D3D12SwapChain final : public AbstractSwapChain
	{
	public:
		D3D12SwapChain(Queue& render_queue, IDXGIFactory5& factory, Window& window, uint8_t buffer_count);

		ImageFormat				 get_format() const override;
		unsigned				 get_current_image_index() const override;
		unsigned				 count_presents() const override;
		unsigned				 count_buffers() const override;
		Extent					 get_size() const override;
		void					 resize(Extent size) override;
		void					 enable_vsync() override;
		void					 disable_vsync() override;
		std::optional<SyncToken> request_frame() override;
		void					 present(Queue& render_queue);
		ID3D12Resource*			 get_back_buffer_ptr(size_t index) const;

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

		void acquire_back_buffers();
	};
}
