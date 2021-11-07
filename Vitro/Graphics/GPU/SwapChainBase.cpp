module;
#include <optional>
export module vt.Graphics.SwapChainBase;

import vt.App.WindowEvent;
import vt.Core.Rect;
import vt.Graphics.AssetResourceSpecification;
import vt.Graphics.Handle;

namespace vt
{
	export class SwapChainBase
	{
	public:
		static constexpr unsigned MAX_BUFFERS = 3;

		virtual ~SwapChainBase() = default;

		virtual ImageFormat get_format() const				= 0;
		virtual unsigned	get_current_image_index() const = 0;
		virtual unsigned	count_presents() const			= 0;
		virtual unsigned	count_buffers() const			= 0;
		virtual unsigned	get_width() const				= 0;
		virtual unsigned	get_height() const				= 0;
		virtual void		resize(Extent size)				= 0;
		virtual void		enable_vsync()					= 0;
		virtual void		disable_vsync()					= 0;

		// To be called when rendering of a frame begins for a specific swap chain. Returns a token that indicates when the next
		// back buffer is ready or nullopt if the operation failed and the swap chain must be resized.
		virtual std::optional<SyncToken> request_frame() = 0;

		bool vsync_enabled() const
		{
			return is_vsync_enabled;
		}

	protected:
		bool is_vsync_enabled = false;
	};
}
