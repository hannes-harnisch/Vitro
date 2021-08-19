module;
#include <algorithm>
#include <atomic>
#include <memory>
#include <ranges>
#include <typeindex>
#include <utility>
#include <vector>
export module Vitro.App.EventSystem;

import Vitro.App.Event;
import Vitro.Core.Singleton;
import Vitro.Trace.Log;

namespace vt
{
	export class EventSystem : public Singleton<EventSystem>
	{
		friend class AppSystem;
		friend class EventListener;

	public:
		static void notify(std::unique_ptr<Event> event)
		{
			get().dispatchEvent(std::move(event));
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
			std::type_index eventType;
		};
		std::vector<EventHandler> handlers;

		EventSystem() = default;

		void dispatchEvent(std::unique_ptr<Event> event)
		{
			std::type_index eventType = typeid(*event);
			for(auto handler : std::views::reverse(handlers))
			{
				if(handler.eventType == eventType)
				{
					bool consumed = handler.function(*handler.listener, *event);
					if(consumed)
						break;
				}
			}
			Log().verbose(event);
		}

		template<typename... Ts> void submitHandler(Ts&&... ts)
		{
			handlers.emplace_back(std::forward<Ts>(ts)...);
		}

		void duplicateHandlersWithListener(EventListener& newListener, EventListener const& oldListener)
		{
			for(auto i = handlers.begin(); i != handlers.end(); ++i)
				if(i->listener == &oldListener)
					i = handlers.emplace(i + 1, i->function, &newListener, i->eventType);
		}

		void replaceListener(EventListener& newListener, EventListener& oldListener)
		{
			for(auto& handler : handlers)
				if(handler.listener == &oldListener)
					handler.listener = &newListener;
		}

		void removeHandlersWithListener(EventListener& listener)
		{
			std::erase_if(handlers, [&](auto handler) {
				return handler.listener == &listener;
			});
		}
	};
}
