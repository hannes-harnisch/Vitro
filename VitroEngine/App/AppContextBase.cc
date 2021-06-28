module;
#include <unordered_map>
export module Vitro.App.AppContextBase;

import Vitro.Core.Singleton;

namespace vt
{
	export class AppContextBase : public Singleton<AppContextBase>
	{
	public:
		virtual void pollEvents() const = 0;
		virtual void* handle()			= 0;

		void notifyWindowConstruction(void* const nativeHandle, class Window& window)
		{
			nativeWindowToEngineWindow[nativeHandle] = &window;
		}

		void notifyWindowDestruction(void* const nativeHandle)
		{
			nativeWindowToEngineWindow.erase(nativeHandle);
		}

		Window* findWindow(void* const nativeHandle)
		{
			auto it = nativeWindowToEngineWindow.find(nativeHandle);

			if(it == nativeWindowToEngineWindow.end())
				return nullptr;

			return it->second;
		}

	private:
		std::unordered_map<void*, Window*> nativeWindowToEngineWindow;
	};
}
