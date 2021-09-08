module;
#include <algorithm>
#include <atomic>
#include <memory>
#include <ranges>
#include <typeindex>
#include <utility>
#include <vector>
export module vt.App.EventSystem;

import vt.App.Event;
import vt.App.WindowEvent;
import vt.Core.Singleton;
import vt.Trace.Log;

namespace vt
{
	export class EventSystem : public Singleton<EventSystem>
	{
		friend class Engine;
		friend class EventListener;

	public:
		static void notify(std::unique_ptr<Event> event)
		{
			get().dispatch_event(std::move(event));
		}

		template<typename TEvent, typename... Ts> static void notify(Ts&&... ts)
		{
			notify(std::make_unique<TEvent>(std::forward<Ts>(ts)...));
		}

	private:
		struct EventHandler
		{
			bool (*function)(class EventListener&, Event&);
			EventListener*	listener;
			std::type_index event_type;
		};
		std::vector<EventHandler> handlers;

		EventSystem() = default;

		void dispatch_event(std::unique_ptr<Event> event)
		{
			std::type_index event_type = typeid(*event);
			for(auto handler : std::views::reverse(handlers))
			{
				if(handler.event_type == event_type)
				{
					bool consumed = handler.function(*handler.listener, *event);
					if(consumed)
						break;
				}
			}
			if(event_type != typeid(WindowPaintEvent))
				Log().verbose(*event);
		}

		template<typename... Ts> void submit_handler(Ts&&... ts)
		{
			handlers.emplace_back(std::forward<Ts>(ts)...);
		}

		void duplicate_handlers_with_listener(EventListener& new_listener, EventListener const& old_listener)
		{
			for(size_t i = 0; i != handlers.size(); ++i)
			{
				auto handler = handlers[i];
				if(handler.listener == &old_listener)
					handlers.emplace_back(handler.function, &new_listener, handler.event_type);
			}
		}

		void replace_listener(EventListener& new_listener, EventListener& old_listener)
		{
			for(auto& handler : handlers)
				if(handler.listener == &old_listener)
					handler.listener = &new_listener;
		}

		void remove_handlers_with_listener(EventListener& listener)
		{
			std::erase_if(handlers, [&](auto handler) {
				return handler.listener == &listener;
			});
		}
	};
}
