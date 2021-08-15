module;
#include <thread>
export module Vitro.App.AppContextBase;

import Vitro.Core.HashMap;
import Vitro.Core.Singleton;

namespace vt
{
	export class AppContextBase : public Singleton<AppContextBase>
	{
	public:
		virtual void  pollEvents() const = 0;
		virtual void* handle()			 = 0;

		std::thread::id mainThreadId()
		{
			return mainThread;
		}

		void notifyWindowMapping(void* nativeHandle, class Window& window)
		{
			nativeWindowToEngineWindow[nativeHandle] = &window;
		}

		void notifyWindowDestruction(void* nativeHandle)
		{
			nativeWindowToEngineWindow.erase(nativeHandle);
		}

	protected:
		Window* findWindow(void* nativeHandle)
		{
			auto it = nativeWindowToEngineWindow.find(nativeHandle);

			if(it == nativeWindowToEngineWindow.end())
				return nullptr;

			return it->second;
		}

	private:
		HashMap<void*, Window*> nativeWindowToEngineWindow;
		std::thread::id			mainThread = std::this_thread::get_id();
	};
}
