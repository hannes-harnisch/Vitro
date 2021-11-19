module;
#include <concepts>
#include <typeinfo>
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

		template<auto HANDLER> void register_event_handler()
		{
			auto& type	  = typeid(EventHandlerTraits<decltype(HANDLER)>::Event);
			auto  handler = dispatch<HANDLER>;
			EventSystem::get().submit_handler(type, handler, this);
		}

		template<auto... HANDLERS> void register_event_handlers()
		{
			(register_event_handler<HANDLERS>(), ...);
		}

		template<auto HANDLER> void unregister_event_handler()
		{
			auto& type	  = typeid(EventHandlerTraits<decltype(HANDLER)>::Event);
			auto  handler = dispatch<HANDLER>;
			EventSystem::get().remove_handler(type, handler, this);
		}

		template<auto... HANDLERS> void unregister_event_handlers()
		{
			(unregister_event_handler<HANDLERS>(), ...);
		}

		void unregister_all_event_handlers()
		{
			EventSystem::get().remove_handlers_with_listener(*this);
		}

	private:
		template<typename> struct EventHandlerTraits;
		template<typename R, typename C, typename E> struct EventHandlerTraits<R (C::*)(E)>
		{
			using Return = R;
			using Class	 = C;
			using Event	 = std::remove_cvref_t<E>;
		};

		template<auto HANDLER> static bool dispatch(EventListener& listener, void* event_data)
		{
			using Handler = EventHandlerTraits<decltype(HANDLER)>;

			// The logic within event system guarantees that the event passed into the lambda will have the type that it is
			// being casted to here.
			auto& event	 = *static_cast<Handler::Event*>(event_data);
			auto& object = static_cast<Handler::Class&>(listener);

			if constexpr(std::same_as<Handler::Return, void>)
			{
				(object.*HANDLER)(event);
				return false;
			}
			else if constexpr(std::same_as<Handler::Return, bool>)
				return (object.*HANDLER)(event);
			else
				static_assert(false, "An event handler must return void or bool.");
		}
	};
}
