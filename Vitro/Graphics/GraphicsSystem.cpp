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
import vt.Graphics.Handle;
import vt.Graphics.RendererBase;
import vt.Graphics.RingBuffer;
import vt.Graphics.SwapChain;
import vt.Trace.Log;

namespace vt
{
	struct WindowContext
	{
		struct FrameResources
		{
			SyncToken final_submit_token = {};
		};
		RingBuffer<FrameResources>	  ring;
		SwapChain					  swap_chain;
		std::unique_ptr<RendererBase> renderer;
		std::atomic_bool			  swap_chain_invalid = false;
		Extent						  window_size;

		WindowContext(Window& window, Device& device) :
			swap_chain(device->make_swap_chain(window)),
			renderer(std::make_unique<ForwardRenderer>(device, swap_chain->get_format()))
		{}

		void update(Window& window, Device& device)
		{
			if(swap_chain_invalid)
			{
				device->flush_render_queue();
				swap_chain->resize(window_size);
				window.enable_resize();

				renderer->recreate_shared_render_targets(swap_chain);
				swap_chain_invalid = false;
			}
			else
				device->wait_for_workload(ring->final_submit_token);

			auto present_token = swap_chain->request_frame();
			if(present_token)
			{
				auto final_commands		 = renderer->render(swap_chain);
				ring->final_submit_token = device->submit_for_present(final_commands, swap_chain, *present_token);
			}
			ring.move_to_next_frame();
		}
	};

	export class GraphicsSystem : public EventListener
	{
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
		std::mutex		 resize_mutex;
		std::jthread	 render_thread;

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
			while(should_run)
			{
				std::lock_guard lock(context_access_mutex);
				for(auto& [window, context] : window_contexts)
					context.update(*window, device);
			}
		}

		void on_window_resize(WindowSizeEvent& window_size_event)
		{
			auto& [window, context] = *window_contexts.find(&window_size_event.window);
			window->disable_resize();
			context.swap_chain_invalid = true;
			context.window_size		   = window_size_event.size;
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
			replace_key_for_swap_chain(window_moved.moved, window_moved.constructed);
		}

		void on_window_object_destroy(ObjectDestroyEvent<Window>& window_destroyed)
		{
			std::lock_guard lock(context_access_mutex);
			destroy_swap_chain(window_destroyed.object);
		}

		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& window_moved)
		{
			std::lock_guard lock(context_access_mutex);

			destroy_swap_chain(window_moved.left);
			replace_key_for_swap_chain(window_moved.right, window_moved.left);
		}

		void destroy_swap_chain(Window& window)
		{
			device->flush(); // Flush entire device because context destroys renderer, which might use multiple queues.
			window_contexts.erase(&window);
		}

		void replace_key_for_swap_chain(Window& old_window, Window& new_window)
		{
			auto node  = window_contexts.extract(&old_window);
			node.key() = &new_window;
			window_contexts.insert(std::move(node));
		}
	};
}
