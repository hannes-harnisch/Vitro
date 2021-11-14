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
export module vt.App.EventSystem;

import vt.Core.ConcurrentQueue;
import vt.Core.Reflect;
import vt.Core.Singleton;
import vt.Trace.Log;

namespace vt
{
	export template<typename T> constexpr bool ALLOW_ASYNC_DISPATCH_FOR = false;

	export class EventSystem : public Singleton<EventSystem>
	{
		friend class AppSystem;
		friend class EventListener;

	public:
		template<typename T, typename... Ts> static void notify(Ts&&... ts)
		{
			T event {std::forward<Ts>(ts)...};
			// Log().verbose(name_of(event), ": ", event);
			get().dispatch_event(&event, typeid(T));
		}

		template<typename T, typename... Ts> static void notify_async(Ts&&... ts)
		{
			static_assert(ALLOW_ASYNC_DISPATCH_FOR<T>, "This type is not allowed to be used as an async event.");
			get().enqueue_async_event(T {std::forward<Ts>(ts)...});
		}

	private:
		using Callback = bool (*)(class EventListener&, void*);
		struct EventHandler
		{
			Callback	   callback;
			EventListener* listener;
		};
		std::unordered_map<std::type_index, std::vector<EventHandler>> event_handlers;

		ConsumerToken			  con_token;
		ConcurrentQueue<std::any> async_events; // TODO: replace with custom any-like type, since we need void*.

		EventSystem() : con_token(async_events)
		{}

		void enqueue_async_event(std::any event)
		{
			thread_local ProducerToken const pro_token(async_events);
			async_events.enqueue(pro_token, std::move(event));
		}

		void dispatch_event(void* event, std::type_index type) const
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

		void submit_handler(std::type_index event_type, Callback callback, EventListener* listener)
		{
			event_handlers[event_type].emplace_back(callback, listener);
		}

		void remove_handler(std::type_index event_type, Callback callback, EventListener* listener)
		{
			auto handlers = event_handlers.find(event_type);
			if(handlers == event_handlers.end())
				return;

			std::erase_if(handlers->second, [=](EventHandler handler) {
				return callback == handler.callback && listener == handler.listener;
			});
		}

		void duplicate_handlers_with_listener(EventListener& new_listener, EventListener const& old_listener)
		{
			for(auto& [type, handlers] : event_handlers)
				for(size_t i = 0; i != handlers.size(); ++i)
				{
					auto handler = handlers[i];
					if(handler.listener == &old_listener)
						handlers.emplace_back(handler.callback, &new_listener);
				}
		}

		void replace_listener(EventListener& new_listener, EventListener& old_listener)
		{
			for(auto& [type, handlers] : event_handlers)
				for(auto& handler : handlers)
					if(handler.listener == &old_listener)
						handler.listener = &new_listener;
		}

		void remove_handlers_with_listener(EventListener& listener)
		{
			for(auto& [type, handlers] : event_handlers)
				std::erase_if(handlers, [&](EventHandler handler) {
					return handler.listener == &listener;
				});
		}
	};
}
