module;
#include "Core/Macros.hpp"

#include <algorithm>
#include <memory>
#include <span>
#include <string>
#include <unordered_map>
export module vt.Graphics.GraphicsSystem;

import vt.App.EventListener;
import vt.App.ObjectEvent;
import vt.App.Window;
import vt.App.WindowEvent;
import vt.Core.Version;
import vt.Graphics.Device;
import vt.Graphics.Driver;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.ForwardRenderer;
import vt.Graphics.RendererBase;
import vt.Graphics.Handle;
import vt.Graphics.SwapChain;

namespace vt
{
	export class GraphicsSystem : public EventListener
	{
	public:
		GraphicsSystem(std::string const& app_name, Version app_version, Version engine_version) :
			driver(app_name, app_version, engine_version),
			device(select_adapter()),
			renderer(std::make_unique<ForwardRenderer>(device))
		{
			register_event_handlers<&GraphicsSystem::on_paint, &GraphicsSystem::on_window_resize,
									&GraphicsSystem::on_window_object_construct,
									&GraphicsSystem::on_window_object_move_construct, &GraphicsSystem::on_window_object_destroy,
									&GraphicsSystem::on_window_object_move_assign>();
		}

	private:
		std::unordered_map<Window const*, SwapChain> swap_chains;

		DynamicGpuApi				  dynamic_gpu_api;
		Driver						  driver;
		Device						  device;
		std::unique_ptr<RendererBase> renderer;

		Adapter select_adapter()
		{
			auto adapters = driver->enumerate_adapters();
			auto selected = std::max_element(adapters.begin(), adapters.end(), [](Adapter const& a1, Adapter const& a2) {
				return a1.get_vram() < a2.get_vram();
			});
			VT_ENSURE(selected != adapters.end(), "No suitable GPUs found.");
			return std::move(*selected);
		}

		void on_paint(WindowPaintEvent& event)
		{
			auto& swap_chain = swap_chains.at(&event.window);
			renderer->draw(swap_chain);
		}

		void on_window_resize(WindowSizeEvent& event)
		{
			static bool is_first_resize = true;
			if(is_first_resize)
			{
				is_first_resize = false;
				return;
			}
			if(event.size.zero())
				return;

			auto& swap_chain = swap_chains.find(&event.window)->second;
			swap_chain->resize(event);
			renderer->recreate_swap_chain_render_targets(swap_chain);
		}

		void on_window_object_construct(ObjectConstructEvent<Window>& window_construct)
		{
			auto& window = window_construct.object;
			swap_chains.try_emplace(&window, driver, device, window);
		}

		void on_window_object_move_construct(ObjectMoveConstructEvent<Window>& window_move)
		{
			replace_key_for_swap_chain(window_move.moved, window_move.constructed);
		}

		void on_window_object_destroy(ObjectDestroyEvent<Window>& window_destroy)
		{
			remove_swap_chain(window_destroy.object);
		}

		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& window_move)
		{
			remove_swap_chain(window_move.left);
			replace_key_for_swap_chain(window_move.right, window_move.left);
		}

		void remove_swap_chain(Window const& window)
		{
			device->flush_render_queue();
			swap_chains.erase(&window);
		}

		void replace_key_for_swap_chain(Window const& old_window, Window const& new_window)
		{
			auto node  = swap_chains.extract(&old_window);
			node.key() = &new_window;
			swap_chains.insert(std::move(node));
		}
	};
}
