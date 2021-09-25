export module vt.Graphics.SwapChainBase;

import vt.App.WindowEvent;

namespace vt
{
	export class SwapChainBase
	{
	public:
		virtual ~SwapChainBase() = default;

		virtual unsigned get_current_image_index() const	  = 0;
		virtual unsigned get_buffer_count() const			  = 0;
		virtual void	 present()							  = 0;
		virtual void	 resize(WindowSizeEvent const& event) = 0;
		virtual void	 enable_vsync()						  = 0;
		virtual void	 disable_vsync()					  = 0;

		bool vsync_enabled() const
		{
			return is_vsync_enabled;
		}

	protected:
		static constexpr unsigned MaxBuffers = 3;

		bool is_vsync_enabled = false;
	};
}
