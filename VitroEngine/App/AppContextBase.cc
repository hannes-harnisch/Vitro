module;
#include <unordered_map>
export module Vitro.App.AppContextBase;

import Vitro.Core.Singleton;

export class AppContextBase : public Singleton<AppContextBase>
{
public:
	virtual void pollEvents() const = 0;
	virtual void* handle()			= 0;

	void submitWindow(void* const nativeHandle, class Window* const window)
	{
		nativeWindowToEngineWindow[nativeHandle] = window;
	}

	void removeWindow(void* const nativeHandle)
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
