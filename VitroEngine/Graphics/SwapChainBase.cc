export module vt.Graphics.SwapChainBase;

import vt.Graphics.RenderTarget;

namespace vt
{
	export class SwapChainBase
	{
	public:
		virtual ~SwapChainBase() = default;

		virtual RenderTarget& acquire_render_target() = 0;
		virtual void		  present()				  = 0;
		virtual void		  resize()				  = 0;
		virtual void		  enable_v_sync()		  = 0;
		virtual void		  disable_v_sync()		  = 0;

		bool v_sync_enabled() const
		{
			return is_v_sync_enabled;
		}

	protected:
		static constexpr unsigned MaxBuffers		 = 3;
		static constexpr unsigned DefaultBufferCount = 3;

		bool is_v_sync_enabled = false;
	};
}
