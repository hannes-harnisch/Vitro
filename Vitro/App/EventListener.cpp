module;
#include <any>
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
			unregister_event_handlers();
		}

		EventListener& operator=(EventListener const& that)
		{
			unregister_event_handlers();
			EventSystem::get().duplicate_handlers_with_listener(*this, that);
			return *this;
		}

		EventListener& operator=(EventListener&& that) noexcept
		{
			unregister_event_handlers();
			EventSystem::get().replace_listener(*this, that);
			return *this;
		}

		template<auto... Handlers> void register_event_handlers()
		{
			(register_handler<Handlers>(), ...);
		}

		void unregister_event_handlers()
		{
			EventSystem::get().remove_handlers_with_listener(*this);
		}

	private:
		template<typename> struct FunctionTraits;
		template<typename R, typename C, typename P> struct FunctionTraits<R (C::*)(P)>
		{
			using Return = R;
			using Class	 = C;
			using Param	 = P;
		};

		template<auto Handler> void register_handler()
		{
			using HandlerTraits = FunctionTraits<decltype(Handler)>;
			using TReturn		= HandlerTraits::Return;
			using TClass		= HandlerTraits::Class;
			using TEvent		= std::remove_cvref_t<HandlerTraits::Param>;

			auto func = [](EventListener& listener, std::any& stored_event) {
				auto& event	 = std::any_cast<TEvent&>(stored_event);
				auto& object = static_cast<TClass&>(listener);
				if constexpr(std::is_same_v<TReturn, void>)
				{
					(object.*Handler)(event);
					return false;
				}
				else
					return (object.*Handler)(event);
			};
			EventSystem::get().submit_handler(typeid(TEvent), func, this);
		}
	};
}
