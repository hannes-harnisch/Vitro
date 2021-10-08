module;
#include "Core/Macros.hpp"

#include <algorithm>
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
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
import vt.Trace.Log;

namespace vt
{
	export class GraphicsSystem : public EventListener
	{
	public:
		GraphicsSystem(std::string const& app_name, Version app_version, Version engine_version) :
			driver(app_name, app_version, engine_version),
			device(select_adapter()),
			renderer(std::make_unique<ForwardRenderer>(device)),
			render_thread(&GraphicsSystem::run_rendering, this)
		{
			register_event_handlers<&GraphicsSystem::on_window_resize, &GraphicsSystem::on_window_object_construct,
									&GraphicsSystem::on_window_object_move_construct, &GraphicsSystem::on_window_object_destroy,
									&GraphicsSystem::on_window_object_move_assign>();
		}

		~GraphicsSystem()
		{
			should_run = false;
		}

	private:
		struct WindowState
		{
			SwapChain			swap_chain;
			std::atomic<Extent> size_to_update;
		};
		std::unordered_map<Window const*, WindowState> window_states; // Iterator stability is needed, hence unordered_map.

		DynamicGpuApi				  dynamic_gpu_api;
		Driver						  driver;
		Device						  device;
		std::unique_ptr<RendererBase> renderer;
		std::atomic_bool			  should_run = true;
		std::mutex					  mutex;
		std::jthread				  render_thread;

		Adapter select_adapter()
		{
			auto adapters = driver->enumerate_adapters();
			auto selected = std::max_element(adapters.begin(), adapters.end(), [](Adapter const& a1, Adapter const& a2) {
				return a1.get_vram() < a2.get_vram();
			});
			VT_ENSURE(selected != adapters.end(), "No suitable GPUs found.");
			return std::move(*selected);
		}

		void run_rendering()
		{
			while(should_run)
			{
				std::unique_lock lock(mutex);
				for(auto& [window, window_state] : window_states)
				{
					auto& swap_chain = window_state.swap_chain;
					if(!window_state.size_to_update.load().zero())
					{
						swap_chain->resize(window_state.size_to_update);
						renderer->recreate_shared_render_targets(swap_chain);
						window_state.size_to_update.store({0, 0});
					}

					renderer->draw(swap_chain);
					swap_chain->present();
				}
			}
		}

		void on_window_resize(WindowSizeEvent& window_size_event)
		{
			auto& window_state = window_states.find(&window_size_event.window)->second;

			window_state.size_to_update = window_size_event.size;
		}

		void on_window_object_construct(ObjectConstructEvent<Window>& window_constructed)
		{
			std::unique_lock lock(mutex);

			auto& window = window_constructed.object;
			window_states.try_emplace(&window, device->make_swap_chain(driver, window));
		}

		void on_window_object_move_construct(ObjectMoveConstructEvent<Window>& window_moved)
		{
			std::unique_lock lock(mutex);
			replace_key_for_swap_chain(window_moved.moved, window_moved.constructed);
		}

		void on_window_object_destroy(ObjectDestroyEvent<Window>& window_destroyed)
		{
			std::unique_lock lock(mutex);
			remove_swap_chain(window_destroyed.object);
		}

		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& window_moved)
		{
			std::unique_lock lock(mutex);
			remove_swap_chain(window_moved.left);
			replace_key_for_swap_chain(window_moved.right, window_moved.left);
		}

		void remove_swap_chain(Window const& window)
		{
			device->flush_render_queue();
			window_states.erase(&window);
		}

		void replace_key_for_swap_chain(Window const& old_window, Window const& new_window)
		{
			auto node  = window_states.extract(&old_window);
			node.key() = &new_window;
			window_states.insert(std::move(node));
		}
	};
}
