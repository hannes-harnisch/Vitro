module;
#include <algorithm>
#include <any>
#include <atomic>
#include <queue>
#include <ranges>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>
export module vt.App.EventSystem;

import vt.Core.Singleton;

namespace vt
{
	export template<typename T> constexpr bool AllowAsyncEventDispatchFor = false;

	export class EventSystem : public Singleton<EventSystem>
	{
		friend class AppSystem;
		friend class EventListener;

	public:
		template<typename T, typename... Ts> static void notify(Ts&&... ts)
		{
			get().dispatch_event(T {std::forward<Ts>(ts)...});
		}

		template<typename T, typename... Ts> static void notify_async(Ts&&... ts)
		{
			if constexpr(!AllowAsyncEventDispatchFor<T>)
				static_assert(false, "This type is not allowed to be used as an async event.");

			get().async_events.emplace(T {std::forward<Ts>(ts)...});
		}

	private:
		using Callback = bool (*)(class EventListener&, std::any&);
		struct EventHandler
		{
			Callback	   callback;
			EventListener* listener;
		};
		std::unordered_map<std::type_index, std::vector<EventHandler>> handlers;

		std::queue<std::any> async_events; // replace any with unsafely castable type?

		EventSystem() = default;

		void dispatch_event(std::any event) const
		{
			auto handlers_for_type = handlers.find(event.type());
			if(handlers_for_type == handlers.end())
				return;

			for(auto handler : std::views::reverse(handlers_for_type->second))
			{
				bool consumed = handler.callback(*handler.listener, event);
				if(consumed)
					break;
			}
		}

		void submit_handler(std::type_index event_type, Callback callback, EventListener* listener)
		{
			handlers[event_type].emplace_back(callback, listener);
		}

		void duplicate_handlers_with_listener(EventListener& new_listener, EventListener const& old_listener)
		{
			for(auto& [type, list] : handlers)
			{
				for(size_t i = 0; i != list.size(); ++i)
				{
					auto handler = list[i];
					if(handler.listener == &old_listener)
						list.emplace_back(handler.callback, &new_listener);
				}
			}
		}

		void replace_listener(EventListener& new_listener, EventListener& old_listener)
		{
			for(auto& [type, list] : handlers)
			{
				for(auto& handler : list)
					if(handler.listener == &old_listener)
						handler.listener = &new_listener;
			}
		}

		void remove_handlers_with_listener(EventListener& listener)
		{
			for(auto& [type, list] : handlers)
			{
				std::erase_if(list, [&](EventHandler handler) {
					return handler.listener == &listener;
				});
			}
		}
	};
}
