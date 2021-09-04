module;
#include <thread>
export module vt.App.AppContextBase;

import vt.Core.HashMap;
import vt.Core.Singleton;

namespace vt
{
	export class AppContextBase : public Singleton<AppContextBase>
	{
	public:
		virtual void  poll_events() const = 0;
		virtual void* get_handle()		  = 0;

		std::thread::id main_thread_id()
		{
			return main_thread;
		}

		void notify_window_mapping(void* native_handle, class Window& window)
		{
			native_window_to_engine_window[native_handle] = &window;
		}

		void notify_window_destruction(void* native_handle)
		{
			native_window_to_engine_window.erase(native_handle);
		}

	protected:
		Window* find_window(void* native_handle)
		{
			auto it = native_window_to_engine_window.find(native_handle);

			if(it == native_window_to_engine_window.end())
				return nullptr;

			return it->second;
		}

	private:
		HashMap<void*, Window*> native_window_to_engine_window;
		std::thread::id			main_thread = std::this_thread::get_id();
	};
}
