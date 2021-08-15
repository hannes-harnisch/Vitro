export module Vitro.Graphics.SwapChainBase;

import Vitro.Graphics.RenderTarget;

namespace vt
{
	export class SwapChainBase
	{
	public:
		virtual ~SwapChainBase() = default;

		virtual RenderTarget& acquireRenderTarget() = 0;
		virtual void		  present()				= 0;
		virtual void		  resize()				= 0;
		virtual void		  enableVSync()			= 0;
		virtual void		  disableVSync()		= 0;

		bool vSyncEnabled() const
		{
			return isVSyncEnabled;
		}

	protected:
		static constexpr unsigned MaxBuffers		 = 3;
		static constexpr unsigned DefaultBufferCount = 3;

		bool isVSyncEnabled = false;
	};
}
