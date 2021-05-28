module;
#include <unordered_map>
export module Vitro.App.AppContextBase;

import Vitro.Core.Singleton;

export class AppContextBase : public Singleton<AppContextBase>
{
public:
	virtual void pollEvents() const = 0;
	virtual void* handle()			= 0;

	void submitWindow(void* nativeHandle, class Window* window)
	{
		nativeWindowToEngineWindow[nativeHandle] = window;
	}

	void removeWindow(void* nativeHandle)
	{
		nativeWindowToEngineWindow.erase(nativeHandle);
	}

	Window* findWindow(void* nativeHandle)
	{
		auto it = nativeWindowToEngineWindow.find(nativeHandle);
		if(it != nativeWindowToEngineWindow.end())
			return it->second;
		else
			return nullptr;
	}

private:
	std::unordered_map<void*, Window*> nativeWindowToEngineWindow;
};
