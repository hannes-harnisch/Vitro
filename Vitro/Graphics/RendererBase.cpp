module;
#include <unordered_map>
#include <vector>
export module vt.Graphics.RendererBase;

import vt.App.ObjectEvent;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;

namespace vt
{
	export class RendererBase : public EventListener
	{
	protected:
		RendererBase(RenderPass present_pass) : present_pass(std::move(present_pass))
		{
			register_event_handlers();
		}

		~RendererBase() = default;

	private:
		std::unordered_map<SwapChain*, std::vector<RenderTarget>> swap_chain_render_targets;

		RenderPass				  present_pass;
		RenderTargetSpecification swap_chain_target_spec;
	};
}
