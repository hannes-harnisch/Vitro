module;
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <ranges>
#include <source_location>
#include <typeindex>
#include <utility>
#include <vector>
export module Vitro.App.EventSystem;

import Vitro.App.Event;
import Vitro.App.EventHandler;
import Vitro.Core.Singleton;
import Vitro.Trace.Log;

namespace vt
{
	export class EventSystem : public Singleton<EventSystem>
	{
		friend class AppSystem;

	public:
		static void notify(std::unique_ptr<Event> event)
		{
			get().emplaceEvent(std::move(event));
		}

		template<typename TEvent, typename... Ts> static void notify(Ts&&... ts)
		{
			get().emplaceEvent(std::make_unique<TEvent>(std::forward<Ts>(ts)...));
		}

		template<typename... Ts> static void submitHandler(Ts&&... ts)
		{
			std::lock_guard lock(get().mutex);
			get().handlers.emplace_back(std::forward<Ts>(ts)...);
		}

		static void removeHandlersByTarget(void* target)
		{
			std::erase_if(get().handlers, [=](EventHandler const& handler) { return handler.callTarget == target; });
		}

	private:
		std::queue<std::unique_ptr<Event>> events;
		std::vector<EventHandler> handlers;
		std::mutex mutex;
		std::condition_variable condition;
		std::atomic_bool isAcceptingEvents = true;

		void emplaceEvent(std::unique_ptr<Event> event)
		{
			{
				std::lock_guard lock(mutex);
				events.emplace(std::move(event));
			}
			condition.notify_one();
		}

		void runEventProcessing()
		{
			while(isAcceptingEvents)
				processQueue();
		}

		void processQueue()
		{
			std::unique_lock lock(mutex);
			condition.wait(lock, [&] { return !events.empty() || !isAcceptingEvents; });
			if(!isAcceptingEvents)
				return;

			auto event = std::move(events.front());
			events.pop();
			lock.unlock();

			processEvent(std::move(event));
		}

		void processEvent(std::unique_ptr<Event> event)
		{
			std::type_index eventType(typeid(*event));

			for(auto&& handler : std::views::reverse(handlers))
			{
				if(handler.eventType == eventType)
				{
					bool consumed = handler.function(*event);
					if(consumed)
						break;
				}
			}
			Log().verbose(event);
		}

		void quit()
		{
			isAcceptingEvents = false;
			condition.notify_all();
		}
	};
}
