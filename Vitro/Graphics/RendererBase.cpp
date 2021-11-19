module;
#include <algorithm>
#include <vector>
module vt.Graphics.RendererBase;

import vt.Core.FixedList;
import vt.Core.Rect;
import vt.Core.SmallList;
import vt.Core.Tick;
import vt.Graphics.Device;
import vt.Graphics.Handle;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;

namespace vt
{
	// For internal use only.
	SmallList<CommandListHandle> RendererBase::render(Tick tick, SwapChain& swap_chain)
	{
		// Create swap chain render targets lazily.
		if(shared_targets.empty())
		{
			auto spec = specify_shared_render_target();

			size_t const count = swap_chain->count_buffers();
			for(int i = 0; i != count; ++i)
				shared_targets.emplace_back(device->make_render_target(spec, swap_chain, i));
		}

		unsigned index		   = swap_chain->get_current_image_index();
		auto&	 render_target = shared_targets[index];
		return render(tick, render_target);
	}

	// For internal use only.
	void RendererBase::recreate_shared_render_targets(SwapChain& swap_chain)
	{
		// First allow renderer to resize its own render targets.
		on_render_target_resize(swap_chain->get_size());

		auto spec = specify_shared_render_target();

		size_t const new_count = swap_chain->count_buffers();
		for(int i = 0; i != std::min(new_count, shared_targets.size()); ++i)
			device->recreate_render_target(shared_targets[i], spec, swap_chain, i);

		if(new_count > shared_targets.size())
		{
			unsigned back_buffer = static_cast<unsigned>(new_count) - 1;
			shared_targets.emplace_back(device->make_render_target(spec, swap_chain, back_buffer));
		}
		else if(new_count < shared_targets.size())
			shared_targets.pop_back();
	}

	RendererBase::RendererBase(Device& device) : device(device)
	{}
}
