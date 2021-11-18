module;
#include <algorithm>
#include <concurrentqueue/concurrentqueue.h>
#include <ranges>
#include <typeinfo>
#include <unordered_map>
#include <vector>
export module vt.App.EventSystem;

import vt.Core.ConcurrentQueue;
import vt.Core.Reflect;
import vt.Trace.Log;

namespace vt
{
	template<typename E> using Callback = bool (*)(class EventListener&, E&);

	class AbstractEventContext
	{
	public:
		virtual void duplicate_handlers_with_listener(EventListener& new_listener, EventListener const& old_listener) = 0;
		virtual void replace_handlers_with_listener(EventListener& new_listener, EventListener& old_listener)		  = 0;
		virtual void remove_handlers_with_listener(EventListener& listener)											  = 0;
		virtual void flush_async_events()																			  = 0;
	};

	std::vector<AbstractEventContext*>& get_event_contexts()
	{
		static std::vector<AbstractEventContext*> contexts;
		return contexts;
	}

	template<typename E> class ConcurrentEventContext
	{};

	template<typename E>
	concept AllowsAsyncDispatch = E::ENABLE_ASYNC_DISPATCH;

	template<AllowsAsyncDispatch E> class ConcurrentEventContext<E>
	{
	public:
		void notify_async(E& event)
		{
			thread_local ProducerToken const pro_token(async_events);
			async_events.enqueue(pro_token, std::move(event));
		}

	protected:
		ConcurrentQueue<E> async_events;
		ConsumerToken	   con_token;

		ConcurrentEventContext() : con_token(async_events)
		{}
	};

	template<typename E> class EventContext : public AbstractEventContext, public ConcurrentEventContext<E>
	{
	public:
		EventContext()
		{
			get_event_contexts().emplace_back(this);
		}

		void duplicate_handlers_with_listener(EventListener& new_listener, EventListener const& old_listener) override
		{
			for(size_t i = 0; i != handlers.size(); ++i)
			{
				auto handler = handlers[i];
				if(handler.listener == &old_listener)
					handlers.emplace_back(handler.callback, &new_listener);
			}
		}

		void replace_handlers_with_listener(EventListener& new_listener, EventListener& old_listener) override
		{
			for(auto& handler : handlers)
				if(handler.listener == &old_listener)
					handler.listener = &new_listener;
		}

		void remove_handlers_with_listener(EventListener& listener) override
		{
			std::erase_if(handlers, [&](Handler handler) {
				return handler.listener == &listener;
			});
		}

		void flush_async_events() override
		{
			if constexpr(AllowsAsyncDispatch<E>)
			{
				constexpr size_t MAX_EVENTS = 20;

				while(this->async_events.size_approx())
					this->async_events.try_consume<MAX_EVENTS>(this->con_token, [&](E& event) {
						dispatch_event(event);
					});
			}
		}

		void add_handler(Callback<E> callback, EventListener* listener)
		{
			handlers.emplace_back(callback, listener);
		}

		void remove_handler(Callback<E> callback, EventListener* listener)
		{
			std::erase_if(handlers, [=](Handler handler) {
				return callback == handler.callback && listener == handler.listener;
			});
		}

		void dispatch(E& event)
		{
			for(auto handler : handlers | std::views::reverse)
			{
				bool consumed = handler.callback(*handler.listener, event);
				if(consumed)
					break;
			}
		}

	private:
		struct Handler
		{
			Callback<E>	   callback;
			EventListener* listener;
		};

		std::vector<Handler> handlers;
	};

	export class EventSystem
	{
		friend class AppSystem;
		friend class EventListener;

	public:
		template<typename E, typename... Ts> static void notify(Ts&&... ts)
		{
			E event {std::forward<Ts>(ts)...};
			Log().verbose(name_of(event), ": ", event);

			event_context<E>.dispatch(event);
		}

		template<typename E, typename... Ts> static void notify_async(Ts&&... ts)
		{
			static_assert(AllowsAsyncDispatch<E>, "This type is not allowed to be used as an async event.");

			E event {std::forward<Ts>(ts)...};
			Log().verbose(name_of(event), ": ", event);

			event_context<E>.notify_async(event);
		}

	private:
		template<typename E> static inline EventContext<E> event_context;

		template<typename E> static void add_handler(Callback<E> callback, EventListener* listener)
		{
			event_context<E>.add_handler(callback, listener);
		}

		template<typename E> static void remove_handler(Callback<E> callback, EventListener* listener)
		{
			event_context<E>.remove_handler(callback, listener);
		}

		static void flush_async_events()
		{
			for(auto context : get_event_contexts())
				context->flush_async_events();
		}

		static void duplicate_handlers_with_listener(EventListener& new_listener, EventListener const& old_listener)
		{
			for(auto context : get_event_contexts())
				context->duplicate_handlers_with_listener(new_listener, old_listener);
		}

		static void replace_listener(EventListener& new_listener, EventListener& old_listener)
		{
			for(auto context : get_event_contexts())
				context->replace_handlers_with_listener(new_listener, old_listener);
		}

		static void remove_handlers_with_listener(EventListener& listener)
		{
			for(auto context : get_event_contexts())
				context->remove_handlers_with_listener(listener);
		}

		EventSystem() = default;
	};
}
