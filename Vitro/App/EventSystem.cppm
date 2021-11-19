module;
#include <any>
#include <concurrentqueue/concurrentqueue.h>
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
	template<typename E>
	concept AllowsAsyncDispatch = E::ALLOW_ASYNC_DISPATCH;

	export class EventSystem : public Singleton<EventSystem>
	{
		friend class AppSystem;
		friend class EventListener;

	public:
		template<typename E, typename... Ts> static void notify(Ts&&... ts)
		{
			E event {std::forward<Ts>(ts)...};
			// Log().verbose(name_of(event), ": ", event);
			get().dispatch_event(&event, typeid(E));
		}

		template<typename E, typename... Ts> static void notify_async(Ts&&... ts)
		{
			static_assert(AllowsAsyncDispatch<E>, "This type is not allowed to be used as an async event.");
			get().enqueue_async_event(E {std::forward<Ts>(ts)...});
		}

	private:
		using Callback = bool (*)(class EventListener&, void*);
		struct EventHandler
		{
			Callback	   callback;
			EventListener* listener;
		};
		std::unordered_map<std::type_index, std::vector<EventHandler>> event_handlers;

		ConcurrentQueue<std::any> async_events; // TODO: replace with custom any-like type, since we need void*.
		ConsumerToken			  con_token;

		EventSystem();

		void enqueue_async_event(std::any event);
		void dispatch_event(void* event, std::type_index type) const;
		void submit_handler(std::type_index event_type, Callback callback, EventListener* listener);
		void remove_handler(std::type_index event_type, Callback callback, EventListener* listener);
		void duplicate_handlers_with_listener(EventListener& new_listener, EventListener const& old_listener);
		void replace_listener(EventListener& new_listener, EventListener& old_listener);
		void remove_handlers_with_listener(EventListener& listener);
	};
}
