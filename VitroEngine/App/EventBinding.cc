export module Vitro.App.EventBinding;

import Vitro.App.Event;
import Vitro.App.EventSystem;

namespace vt
{
	export class EventBinding
	{
	public:
		template<typename... THandlers> EventBinding(void* target, THandlers... handlers) : target(target)
		{
			(submitHandler(handlers), ...);
		}

		~EventBinding()
		{
			EventSystem::removeHandlersByTarget(target);
		}

		EventBinding(EventBinding const&) = delete;
		EventBinding& operator=(EventBinding const&) = delete;

	private:
		void* target;

		template<typename TClass, typename TEvent> void submitHandler(bool (TClass::*handler)(TEvent&))
		{
			auto func = [=](Event& e) {
				auto& event = static_cast<TEvent&>(e);
				return (static_cast<TClass*>(target)->*handler)(event);
			};
			EventSystem::submitHandler(func, typeid(TEvent), target);
		}

		template<typename TClass, typename TEvent> void submitHandler(void (TClass::*handler)(TEvent&))
		{
			auto func = [=](Event& e) {
				auto& event = static_cast<TEvent&>(e);
				(static_cast<TClass*>(target)->*handler)(event);
				return false;
			};
			EventSystem::submitHandler(func, typeid(TEvent), target);
		}
	};
}
