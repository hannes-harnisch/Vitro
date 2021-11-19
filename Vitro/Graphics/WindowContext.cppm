module;
#include <atomic>
#include <memory>
export module vt.Graphics.WindowContext;

import vt.App.Window;
import vt.App.WindowEvent;
import vt.Core.Rect;
import vt.Core.Tick;
import vt.Graphics.Device;
import vt.Graphics.ForwardRenderer;
import vt.Graphics.Handle;
import vt.Graphics.RendererBase;
import vt.Graphics.RingBuffer;
import vt.Graphics.SwapChain;

namespace vt
{
	export class WindowContext
	{
	public:
		WindowContext(Window& window, Device& device);

		void execute_frame(Tick tick, Window& window, Device& device);
		void invalidate_swap_chain(WindowSizeEvent& window_size_event);

	private:
		SwapChain					  swap_chain;
		RingBuffer<SyncToken>		  buffered_final_submit_tokens;
		std::unique_ptr<RendererBase> renderer;
		std::atomic_bool			  swap_chain_invalid = false;
		Extent						  window_size;

		void recreate_swap_chain_state(Device& device, Window& window);
	};
}
