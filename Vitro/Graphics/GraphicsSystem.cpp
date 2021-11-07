module;
#include "Core/Macros.hpp"

#include <algorithm>
#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
export module vt.Graphics.GraphicsSystem;

import vt.App.AppContextBase;
import vt.App.EventListener;
import vt.App.ObjectEvent;
import vt.App.Window;
import vt.App.WindowEvent;
import vt.Core.Tick;
import vt.Core.Version;
import vt.Graphics.Device;
import vt.Graphics.Driver;
import vt.Graphics.DynamicGpuApi;
import vt.Graphics.Handle;
import vt.Graphics.WindowContext;
import vt.Trace.Log;

namespace vt
{
	export class GraphicsSystem : public EventListener
	{
		friend class Engine;

	public:
		GraphicsSystem(bool				  enable_driver_debug_layer,
					   std::string const& app_name,
					   Version			  app_version,
					   Version			  engine_version) :
			driver(enable_driver_debug_layer, app_name, app_version, engine_version),
			device(driver->make_device(select_adapter())),
			render_thread(&GraphicsSystem::run_rendering, this)
		{
			register_event_handlers<&GraphicsSystem::on_window_resize, &GraphicsSystem::on_window_object_construct,
									&GraphicsSystem::on_window_object_move_construct, &GraphicsSystem::on_window_object_destroy,
									&GraphicsSystem::on_window_object_move_assign>();

			if(enable_driver_debug_layer)
				Log().info("GPU driver debug layers enabled.");
		}

		~GraphicsSystem()
		{
			should_run = false;
		}

	private:
		std::unordered_map<Window*, WindowContext> window_contexts; // Iterator stability is needed, hence unordered_map.

		DynamicGpuApi	 dynamic_gpu_api;
		Driver			 driver;
		Device			 device;
		std::atomic_bool should_run = true;
		std::mutex		 context_access_mutex;
		std::jthread	 render_thread;
		Tick			 tick;

		Adapter select_adapter()
		{
			auto adapters = driver->enumerate_adapters();
			auto selected = std::max_element(adapters.begin(), adapters.end(), [](Adapter const& first, Adapter const& second) {
				return first.get_vram() < second.get_vram();
			});
			VT_ENSURE(selected != adapters.end(), "No suitable GPUs found.");
			return std::move(*selected);
		}

		void run_rendering()
		{
			uint64_t previous_time = Tick::measure_time();
			while(should_run)
			{
				tick.update(previous_time);

				std::lock_guard lock(context_access_mutex);
				for(auto& [window, context] : window_contexts)
					context.execute_frame(tick, *window, device);
			}
		}

		void on_window_resize(WindowSizeEvent& window_size_event)
		{
			auto& [window, context] = *window_contexts.find(&window_size_event.window);

			// We're disabling resize here from the event thread, but we'll re-enable it on the other thread after handling the
			// resize.
			window->disable_resize();
			context.invalidate_swap_chain(window_size_event);
		}

		void on_window_object_construct(ObjectConstructEvent<Window>& window_constructed)
		{
			std::lock_guard lock(context_access_mutex);

			auto& window = window_constructed.object;
			window_contexts.try_emplace(&window, window, device);
		}

		void on_window_object_move_construct(ObjectMoveConstructEvent<Window>& window_moved)
		{
			std::lock_guard lock(context_access_mutex);
			replace_key_for_window_context(window_moved.moved, window_moved.constructed);
		}

		void on_window_object_destroy(ObjectDestroyEvent<Window>& window_destroyed)
		{
			std::lock_guard lock(context_access_mutex);
			destroy_window_context(window_destroyed.object);
		}

		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& window_moved)
		{
			std::lock_guard lock(context_access_mutex);

			destroy_window_context(window_moved.left);
			replace_key_for_window_context(window_moved.right, window_moved.left);
		}

		void destroy_window_context(Window& window)
		{
			device->flush(); // Flush entire device because context destroys renderer, which might use multiple queues.
			window_contexts.erase(&window);
		}

		void replace_key_for_window_context(Window& old_window, Window& new_window)
		{
			auto node  = window_contexts.extract(&old_window);
			node.key() = &new_window;
			window_contexts.insert(std::move(node));
		}
	};
}
