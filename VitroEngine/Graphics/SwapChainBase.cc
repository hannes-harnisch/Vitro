export module vt.Graphics.SwapChainBase;

import vt.App.WindowEvent;
import vt.Graphics.RenderTarget;

namespace vt
{
	export class SwapChainBase
	{
	public:
		virtual ~SwapChainBase() = default;

		virtual RenderTarget const& acquire_render_target()				 = 0;
		virtual void				present()							 = 0;
		virtual void				resize(WindowSizeEvent const& event) = 0;
		virtual void				enable_vsync()						 = 0;
		virtual void				disable_vsync()						 = 0;

		bool vsync_enabled() const
		{
			return is_vsync_enabled;
		}

	protected:
		static constexpr unsigned char MaxBuffers		  = 3;
		static constexpr unsigned char DefaultBufferCount = 3;

		bool is_vsync_enabled = false;
	};
}
