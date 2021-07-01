module;
#include <atomic>
#include <condition_variable>
#include <functional>
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
import Vitro.Core.Unique;
import Vitro.Trace.Log;

namespace vt
{
	export class EventSystem : public Singleton<EventSystem>
	{
		friend class AppSystem;

	public:
		void notify(Unique<Event> event)
		{
			{
				std::lock_guard const lock(mutex);
				events.emplace(std::move(event));
			}
			condition.notify_one();
		}

		template<typename E, typename... Ts> void notify(Ts&&... ts)
		{
			notify(Unique<E>::from(std::forward<Ts>(ts)...));
		}

		template<typename... Ts> void submitHandler(Ts&&... ts)
		{
			std::lock_guard const lock(mutex);
			handlers.emplace_back(std::forward<Ts>(ts)...);
		}

		void removeHandlersByTarget(void* target)
		{
			std::erase_if(handlers, [=](EventHandler const& handler) { return handler.callTarget == target; });
		}

	private:
		std::queue<Unique<Event>> events;
		std::vector<EventHandler> handlers;
		std::mutex mutex;
		std::condition_variable condition;
		std::atomic_bool isAcceptingEvents = true;

		void dispatchEventProcessing()
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

		void processEvent(Unique<Event> event)
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
