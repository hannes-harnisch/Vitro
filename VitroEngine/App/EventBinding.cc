export module Vitro.App.EventBinding;

import Vitro.App.Event;
import Vitro.App.EventSystem;

namespace vt
{
	export class EventBinding
	{
	public:
		template<typename... Hs> EventBinding(void* target, Hs... handlers) : target(target)
		{
			(submitHandler(handlers), ...);
		}

		~EventBinding()
		{
			EventSystem::get().removeHandlersByTarget(target);
		}

	private:
		void* target;

		template<typename C, typename E> void submitHandler(bool (C::*handler)(E&))
		{
			auto func = [=](Event& e) {
				E& event = static_cast<E&>(e);
				return (static_cast<C*>(target)->*handler)(event);
			};
			EventSystem::get().submitHandler(func, typeid(E), target);
		}

		template<typename C, typename E> void submitHandler(void (C::*handler)(E&))
		{
			auto func = [=](Event& e) {
				E& event = static_cast<E&>(e);
				(static_cast<C*>(target)->*handler)(event);
				return false;
			};
			EventSystem::get().submitHandler(func, typeid(E), target);
		}
	};
}
