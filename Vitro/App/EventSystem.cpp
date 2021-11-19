module;
#include <algorithm>
#include <any>
#include <atomic>
#include <concurrentqueue/concurrentqueue.h>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>
module vt.App.EventSystem;

import vt.Core.ConcurrentQueue;
import vt.Core.Reflect;
import vt.Core.Singleton;
import vt.Trace.Log;

namespace vt
{
	EventSystem::EventSystem() : con_token(async_events)
	{}

	void EventSystem::enqueue_async_event(std::any event)
	{
		thread_local ProducerToken const pro_token(async_events);
		async_events.enqueue(pro_token, std::move(event));
	}

	void EventSystem::dispatch_event(void* event, std::type_index type) const
	{
		auto handlers = event_handlers.find(type);
		if(handlers == event_handlers.end())
			return;

		for(auto handler : handlers->second | std::views::reverse)
		{
			bool consumed = handler.callback(*handler.listener, event);
			if(consumed)
				break;
		}
	}

	void EventSystem::submit_handler(std::type_index event_type, Callback callback, EventListener* listener)
	{
		event_handlers[event_type].emplace_back(callback, listener);
	}

	void EventSystem::remove_handler(std::type_index event_type, Callback callback, EventListener* listener)
	{
		auto handlers = event_handlers.find(event_type);
		if(handlers == event_handlers.end())
			return;

		std::erase_if(handlers->second, [=](EventHandler handler) {
			return callback == handler.callback && listener == handler.listener;
		});
	}

	void EventSystem::duplicate_handlers_with_listener(EventListener& new_listener, EventListener const& old_listener)
	{
		for(auto& [type, handlers] : event_handlers)
			for(size_t i = 0; i != handlers.size(); ++i)
			{
				auto handler = handlers[i];
				if(handler.listener == &old_listener)
					handlers.emplace_back(handler.callback, &new_listener);
			}
	}

	void EventSystem::replace_listener(EventListener& new_listener, EventListener& old_listener)
	{
		for(auto& [type, handlers] : event_handlers)
			for(auto& handler : handlers)
				if(handler.listener == &old_listener)
					handler.listener = &new_listener;
	}

	void EventSystem::remove_handlers_with_listener(EventListener& listener)
	{
		for(auto& [type, handlers] : event_handlers)
			std::erase_if(handlers, [&](EventHandler handler) {
				return handler.listener == &listener;
			});
	}
}
