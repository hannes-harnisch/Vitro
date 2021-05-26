module;
#include <unordered_map>
export module Vitro.App.IAppContext;

import Vitro.Core.Singleton;

export class IAppContext : public Singleton<IAppContext>
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
		if(nativeWindowToEngineWindow.contains(nativeHandle))
			return nativeWindowToEngineWindow[nativeHandle];
		else
			return nullptr;
	}

private:
	std::unordered_map<void*, Window*> nativeWindowToEngineWindow;
};
