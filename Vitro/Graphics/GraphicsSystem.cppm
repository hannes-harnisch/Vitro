module;
#include "VitroCore/Macros.hpp"

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

namespace vt
{
	export class GraphicsSystem : public EventListener
	{
		friend class Engine;

	public:
		GraphicsSystem(bool				  enable_driver_debug_layer,
					   std::string const& app_name,
					   Version			  app_version,
					   Version			  engine_version);

		~GraphicsSystem();

	private:
		std::unordered_map<Window*, WindowContext> window_contexts; // Iterator stability is needed, hence unordered_map.

		DynamicGpuApi	 dynamic_gpu_api;
		Driver			 driver;
		Device			 device;
		std::atomic_bool should_run = true;
		std::mutex		 context_access_mutex;
		std::jthread	 render_thread;
		Tick			 tick;

		Adapter select_adapter();
		void	run_rendering();
		void	on_window_resize(WindowSizeEvent& window_size_event);
		void	on_window_object_construct(ObjectConstructEvent<Window>& window_constructed);
		void	on_window_object_move_construct(ObjectMoveConstructEvent<Window>& window_moved);
		void	on_window_object_destroy(ObjectDestroyEvent<Window>& window_destroyed);
		void	on_window_object_move_assign(ObjectMoveAssignEvent<Window>& window_moved);
		void	destroy_window_context(Window& window);
		void	replace_key_for_window_context(Window& old_window, Window& new_window);
	};
}
