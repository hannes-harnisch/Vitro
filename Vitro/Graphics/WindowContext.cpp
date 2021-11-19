module;
#include <atomic>
#include <memory>
#include <vector>
module vt.Graphics.WindowContext;

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
	WindowContext::WindowContext(Window& window, Device& device) :
		swap_chain(device->make_swap_chain(window)),
		renderer(std::make_unique<ForwardRenderer>(device, window.client_area().extent(), swap_chain->get_format()))
	{}

	void WindowContext::execute_frame(Tick tick, Window& window, Device& device)
	{
		auto& current_token = buffered_final_submit_tokens.current();
		if(swap_chain_invalid)
			recreate_swap_chain_state(device, window);
		else
			device->wait_for_workload(current_token);

		auto present_token = swap_chain->request_frame();
		if(present_token)
		{
			auto final_commands = renderer->render(tick, swap_chain);
			current_token		= device->submit_for_present(final_commands, swap_chain, *present_token);
		}
		buffered_final_submit_tokens.move_to_next_frame();
	}

	void WindowContext::invalidate_swap_chain(WindowSizeEvent& window_size_event)
	{
		swap_chain_invalid = true;
		window_size		   = window_size_event.size;
	}
	void WindowContext::recreate_swap_chain_state(Device& device, Window& window)
	{
		device->flush_render_queue();
		swap_chain->resize(window_size);
		renderer->recreate_shared_render_targets(swap_chain);

		window.enable_resize();
		swap_chain_invalid = false;
	}
}
