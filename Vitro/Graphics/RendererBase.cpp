module;
#include <algorithm>
#include <unordered_map>
export module vt.Graphics.RendererBase;

import vt.App.EventListener;
import vt.App.ObjectEvent;
import vt.Core.FixedList;
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

		virtual void draw(SwapChain& swap_chain) = 0;

		void recreate_swap_chain_render_targets(SwapChain& swap_chain)
		{
			swap_chain_target_spec.swap_chain = &swap_chain;

			size_t const new_count = swap_chain->get_buffer_count();

			auto& targets = swap_chain_render_targets.find(&swap_chain)->second;
			for(unsigned i = 0; i != std::min(new_count, targets.size()); ++i)
			{
				swap_chain_target_spec.swap_chain_src_index = i;
				targets[i].reset(swap_chain_target_spec);
			}

			if(new_count > targets.size())
			{
				swap_chain_target_spec.swap_chain_src_index = static_cast<unsigned>(new_count) - 1;
				targets.emplace_back(device, swap_chain_target_spec);
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

		void register_render_target_specification(RenderTargetSpecification const& spec)
		{
			swap_chain_target_spec = spec;
		}

		RenderTarget const& get_swap_chain_target(SwapChain& swap_chain, unsigned index) const
		{
			return swap_chain_render_targets.find(&swap_chain)->second[index];
		}

	private:
		using RenderTargetList = FixedList<RenderTarget, SwapChainBase::MaxBuffers>;

		RenderTargetSpecification						 swap_chain_target_spec;
		std::unordered_map<SwapChain*, RenderTargetList> swap_chain_render_targets;

		void on_swap_chain_construct(ObjectConstructEvent<SwapChain>& swap_chain_construct)
		{
			auto  swap_chain = &swap_chain_construct.object;
			auto& targets	 = swap_chain_render_targets.try_emplace(swap_chain).first->second;

			swap_chain_target_spec.swap_chain = swap_chain;

			unsigned const count = swap_chain_construct.object->get_buffer_count();
			for(unsigned i = 0; i != count; ++i)
			{
				swap_chain_target_spec.swap_chain_src_index = i;
				targets.emplace_back(device, swap_chain_target_spec);
			}
		}

		void on_swap_chain_move_construct(ObjectMoveConstructEvent<SwapChain>& swap_chain_move)
		{
			replace_key_for_render_targets(swap_chain_move.moved, swap_chain_move.constructed);
		}

		void on_swap_chain_destroy(ObjectDestroyEvent<SwapChain>& swap_chain_destroy)
		{
			remove_render_targets(swap_chain_destroy.object);
		}

		void on_swap_chain_move_assign(ObjectMoveAssignEvent<SwapChain>& swap_chain_move)
		{
			remove_render_targets(swap_chain_move.left);
			replace_key_for_render_targets(swap_chain_move.right, swap_chain_move.left);
		}

		void remove_render_targets(SwapChain& swap_chain)
		{
			swap_chain_render_targets.erase(&swap_chain);
		}

		void replace_key_for_render_targets(SwapChain& old_swap_chain, SwapChain& new_swap_chain)
		{
			auto node  = swap_chain_render_targets.extract(&old_swap_chain);
			node.key() = &new_swap_chain;
			swap_chain_render_targets.insert(std::move(node));
		}
	};
}
