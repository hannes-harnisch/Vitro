module;
#include <type_traits>
export module vt.App.EventListener;

import vt.App.EventSystem;

namespace vt
{
	export class EventListener
	{
	protected:
		EventListener() = default;

		EventListener(EventListener const& that)
		{
			EventSystem::get().duplicate_handlers_with_listener(*this, that);
		}

		EventListener(EventListener&& that) noexcept
		{
			EventSystem::get().replace_listener(*this, that);
		}

		~EventListener()
		{
			unregister_all_event_handlers();
		}

		EventListener& operator=(EventListener const& that)
		{
			unregister_all_event_handlers();
			EventSystem::get().duplicate_handlers_with_listener(*this, that);
			return *this;
		}

		EventListener& operator=(EventListener&& that) noexcept
		{
			unregister_all_event_handlers();
			EventSystem::get().replace_listener(*this, that);
			return *this;
		}

		template<auto... HANDLERS> void register_event_handlers()
		{
			(register_handler<HANDLERS>(), ...);
		}

		void unregister_all_event_handlers()
		{
			EventSystem::get().remove_handlers_with_listener(*this);
		}

	private:
		template<typename> struct EventHandlerTraits;
		template<typename R, typename C, typename P> struct EventHandlerTraits<R (C::*)(P)>
		{
			using ReturnType = R;
			using ClassType	 = C;
			using EventType	 = std::remove_cvref_t<P>;
		};

		template<auto HANDLER> void register_handler()
		{
			using Handler = EventHandlerTraits<decltype(HANDLER)>;

			auto func = [](EventListener& listener, void* event_ptr) {
				// The logic within event system guarantees that the event passed into the lambda will have the type that it is
				// being casted to here.
				auto& event	 = *static_cast<Handler::EventType*>(event_ptr);
				auto& object = static_cast<Handler::ClassType&>(listener);

				if constexpr(std::is_same_v<Handler::ReturnType, void>)
				{
					(object.*HANDLER)(event);
					return false;
				}
				else if constexpr(std::is_same_v<Handler::ReturnType, bool>)
					return (object.*HANDLER)(event);
				else
					static_assert(false, "An event handler must return void or bool.");
			};
			EventSystem::get().submit_handler(typeid(Handler::EventType), func, this);
		}
	};
}
