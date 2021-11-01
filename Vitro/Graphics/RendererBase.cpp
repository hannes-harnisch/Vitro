module;
#include <algorithm>
#include <vector>
export module vt.Graphics.RendererBase;

import vt.Core.FixedList;
import vt.Core.Rect;
import vt.Core.SmallList;
import vt.Graphics.Device;
import vt.Graphics.Handle;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;

namespace vt
{
	export class RendererBase
	{
	public:
		virtual ~RendererBase() = default;

		// For internal use only.
		SmallList<CommandListHandle> render(SwapChain& swap_chain)
		{
			// Create swap chain render targets lazily.
			if(shared_targets.empty())
			{
				auto shared_target_spec = get_shared_render_target_specification();

				size_t const count = swap_chain->get_buffer_count();
				for(int i = 0; i != count; ++i)
					shared_targets.emplace_back(device->make_render_target(shared_target_spec, swap_chain, i));
			}

			unsigned index		   = swap_chain->get_current_image_index();
			auto&	 render_target = shared_targets[index];
			return render(render_target);
		}

		// For internal use only.
		void recreate_shared_render_targets(SwapChain& swap_chain)
		{
			// First allow renderer to resize its own render targets.
			on_render_target_resize(swap_chain->get_width(), swap_chain->get_height());

			auto shared_target_spec = get_shared_render_target_specification();

			size_t const new_count = swap_chain->get_buffer_count();
			for(int i = 0; i != std::min(new_count, shared_targets.size()); ++i)
				device->recreate_render_target(shared_targets[i], shared_target_spec, swap_chain, i);

			if(new_count > shared_targets.size())
			{
				unsigned back_buffer = static_cast<unsigned>(new_count) - 1;
				shared_targets.emplace_back(device->make_render_target(shared_target_spec, swap_chain, back_buffer));
			}
			else if(new_count < shared_targets.size())
				shared_targets.pop_back();
		}

	protected:
		Device& device;

		RendererBase(Device& device) : device(device)
		{}

		// Does the renderer-specific rendering. The final batch of command lists is returned for submission here.
		virtual SmallList<CommandListHandle> render(RenderTarget const& render_target) = 0;

		// Gets the specification describing the components of the render target that is last rendered to in this renderer.
		virtual SharedRenderTargetSpecification get_shared_render_target_specification() const = 0;

		// Allows the renderer to respond to swap chain resizing, such as resizing intermediate render targets.
		virtual void on_render_target_resize(unsigned width, unsigned height) = 0;

	private:
		FixedList<RenderTarget, SwapChainBase::MAX_BUFFERS> shared_targets;
	};
}
