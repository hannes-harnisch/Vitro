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
import vt.Graphics.RingBuffer;
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
		struct WindowContext
		{
			struct FrameResources
			{
				SyncToken present_token;
			};
			RingBuffer<FrameResources>	  buffered;
			SwapChain					  swap_chain;
			std::atomic<Extent>			  size_to_update;
			std::unique_ptr<RendererBase> renderer;

			WindowContext(SwapChain swap_chain, std::unique_ptr<RendererBase> renderer) :
				swap_chain(std::move(swap_chain)), renderer(std::move(renderer))
			{}
		};
		std::unordered_map<Window const*, WindowContext> window_contexts; // Iterator stability is needed, hence unordered_map.

		DynamicGpuApi	 dynamic_gpu_api;
		Driver			 driver;
		Device			 device;
		std::atomic_bool should_run = true;
		std::mutex		 mutex;
		std::jthread	 render_thread;

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
				std::lock_guard lock(mutex);
				for(auto& [window, context] : window_contexts)
				{
					auto& swap_chain	= context.swap_chain;
					auto& present_token = context.buffered->present_token;

					if(!context.size_to_update.load().zero())
					{
						device->wait_for_workload(context.buffered.get_previous().present_token);
						swap_chain->resize(context.size_to_update);
						context.renderer->recreate_shared_render_targets(swap_chain);
						context.size_to_update.store({0, 0});
					}
					else
						device->wait_for_workload(present_token);

					auto commands = context.renderer->render(swap_chain);
					present_token = device->submit_for_present(commands, swap_chain);
					context.buffered.move_to_next_frame();
				}
			}
		}

		void on_window_resize(WindowSizeEvent& window_size_event)
		{
			auto& context = window_contexts.find(&window_size_event.window)->second;

			context.size_to_update = window_size_event.size;
		}

		void on_window_object_construct(ObjectConstructEvent<Window>& window_constructed)
		{
			std::lock_guard lock(mutex);

			auto& window = window_constructed.object;
			window_contexts.try_emplace(&window, device->make_swap_chain(driver, window),
										std::make_unique<ForwardRenderer>(device));
		}

		void on_window_object_move_construct(ObjectMoveConstructEvent<Window>& window_moved)
		{
			std::lock_guard lock(mutex);
			replace_key_for_swap_chain(window_moved.moved, window_moved.constructed);
		}

		void on_window_object_destroy(ObjectDestroyEvent<Window>& window_destroyed)
		{
			std::lock_guard lock(mutex);
			remove_swap_chain(window_destroyed.object);
		}

		void on_window_object_move_assign(ObjectMoveAssignEvent<Window>& window_moved)
		{
			std::lock_guard lock(mutex);
			remove_swap_chain(window_moved.left);
			replace_key_for_swap_chain(window_moved.right, window_moved.left);
		}

		void remove_swap_chain(Window const& window)
		{
			device->flush_render_queue();
			window_contexts.erase(&window);
		}

		void replace_key_for_swap_chain(Window const& old_window, Window const& new_window)
		{
			auto node  = window_contexts.extract(&old_window);
			node.key() = &new_window;
			window_contexts.insert(std::move(node));
		}
	};
}
