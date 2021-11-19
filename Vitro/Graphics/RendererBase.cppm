export module vt.Graphics.RendererBase;

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
	export class RendererBase
	{
	public:
		virtual ~RendererBase() = default;

		// For internal use only.
		SmallList<CommandListHandle> render(Tick tick, SwapChain& swap_chain);

		// For internal use only.
		void recreate_shared_render_targets(SwapChain& swap_chain);

	protected:
		Device& device;

		RendererBase(Device& device);

		// Execute renderer-specific rendering. The final batch of command lists is returned for submission here.
		virtual SmallList<CommandListHandle> render(Tick tick, RenderTarget const& render_target) = 0;

		// Gets the specification describing the components of the render target that is rendered to last in a renderer.
		virtual SharedRenderTargetSpecification specify_shared_render_target() const = 0;

		// Allows the renderer to respond to swap chain resizing, such as resizing intermediate render targets.
		virtual void on_render_target_resize(Extent size) = 0;

	private:
		FixedList<RenderTarget, AbstractSwapChain::MAX_BUFFERS> shared_targets;
	};
}
