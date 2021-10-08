module;
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <vector>
export module vt.Graphics.RendererBase;

import vt.App.EventListener;
import vt.App.ObjectEvent;
import vt.Core.FixedList;
import vt.Core.Rectangle;
import vt.Graphics.Device;
import vt.Graphics.RenderPass;
import vt.Graphics.RenderTarget;
import vt.Graphics.RenderTargetSpecification;
import vt.Graphics.SwapChain;

namespace vt
{
	export class RendererBase : public EventListener
	{
	public:
		virtual ~RendererBase() = default;

		void draw(SwapChain& swap_chain)
		{
			unsigned index		   = swap_chain->get_current_image_index();
			auto&	 render_target = shared_render_targets.find(&swap_chain)->second[index];
			draw(render_target);
		}

		void recreate_shared_render_targets(SwapChain& swap_chain)
		{
			on_render_target_resize(swap_chain->get_width(), swap_chain->get_height());
			auto		 shared_target_spec = get_shared_render_target_specification();
			size_t const new_count			= swap_chain->get_buffer_count();

			auto& targets = shared_render_targets.find(&swap_chain)->second;
			for(unsigned i = 0; i != std::min(new_count, targets.size()); ++i)
				device->recreate_render_target(targets[i], shared_target_spec, swap_chain, i);

			if(new_count > targets.size())
			{
				unsigned back_buffer_index = static_cast<unsigned>(new_count) - 1;
				targets.emplace_back(device->make_render_target(shared_target_spec, swap_chain, back_buffer_index));
			}
			else if(new_count < targets.size())
				targets.pop_back();
		}

	protected:
		Device& device;

		RendererBase(Device& device) : device(device)
		{
			register_event_handlers<&RendererBase::on_swap_chain_construct, &RendererBase::on_swap_chain_move_construct,
									&RendererBase::on_swap_chain_destroy, &RendererBase::on_swap_chain_move_assign>();
		}

		virtual void							draw(RenderTarget const& render_target)					 = 0;
		virtual void							on_render_target_resize(unsigned width, unsigned height) = 0;
		virtual SharedRenderTargetSpecification get_shared_render_target_specification() const			 = 0;

	private:
		using RenderTargetList = FixedList<RenderTarget, SwapChainBase::MAX_BUFFERS>;
		std::unordered_map<SwapChain*, RenderTargetList> shared_render_targets;

		void on_swap_chain_construct(ObjectConstructEvent<SwapChain>& swap_chain_constructed)
		{
			auto& swap_chain = swap_chain_constructed.object;
			auto& targets	 = shared_render_targets.try_emplace(&swap_chain).first->second;

			auto shared_target_spec = get_shared_render_target_specification();

			unsigned const count = swap_chain->get_buffer_count();
			for(unsigned i = 0; i != count; ++i)
				targets.emplace_back(device->make_render_target(shared_target_spec, swap_chain, i));
		}

		void on_swap_chain_move_construct(ObjectMoveConstructEvent<SwapChain>& swap_chain_moved)
		{
			replace_key_for_render_targets(swap_chain_moved.moved, swap_chain_moved.constructed);
		}

		void on_swap_chain_destroy(ObjectDestroyEvent<SwapChain>& swap_chain_destroyed)
		{
			remove_render_targets(swap_chain_destroyed.object);
		}

		void on_swap_chain_move_assign(ObjectMoveAssignEvent<SwapChain>& swap_chain_moved)
		{
			remove_render_targets(swap_chain_moved.left);
			replace_key_for_render_targets(swap_chain_moved.right, swap_chain_moved.left);
		}

		void remove_render_targets(SwapChain& swap_chain)
		{
			shared_render_targets.erase(&swap_chain);
		}

		void replace_key_for_render_targets(SwapChain& old_swap_chain, SwapChain& new_swap_chain)
		{
			auto node  = shared_render_targets.extract(&old_swap_chain);
			node.key() = &new_swap_chain;
			shared_render_targets.insert(std::move(node));
		}
	};
}
