module;
#include <functional>
#include <typeindex>
export module Vitro.App.EventHandler;

import Vitro.App.Event;

export struct EventHandler
{
	std::function<bool(Event&)> function;
	std::type_index eventType;
	void* callTarget;
};
