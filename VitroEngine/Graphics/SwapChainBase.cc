export module Vitro.Graphics.SwapChainBase;

namespace vt
{
	export class SwapChainBase
	{
	public:
		virtual unsigned queryRenderTargetIndex() = 0;
		virtual void present()					  = 0;
		virtual void resize()					  = 0;
		virtual void enableVSync()				  = 0;
		virtual void disableVSync()				  = 0;

		bool vSyncEnabled() const
		{
			return isVSyncEnabled;
		}

	protected:
		bool isVSyncEnabled = false;
	};
}
