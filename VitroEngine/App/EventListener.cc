module;
#include <type_traits>
export module Vitro.App.EventListener;

import Vitro.App.EventSystem;
import Vitro.App.Event;

namespace vt
{
	export class EventListener
	{
	public:
		EventListener() = default;

		EventListener(EventListener const& that)
		{
			EventSystem::get().duplicateHandlersWithListener(*this, that);
		}

		EventListener(EventListener&& that) noexcept
		{
			EventSystem::get().replaceListener(*this, that);
		}

		~EventListener()
		{
			EventSystem::get().removeHandlersWithListener(*this);
		}

		EventListener& operator=(EventListener const& that)
		{
			EventSystem::get().removeHandlersWithListener(*this);
			EventSystem::get().duplicateHandlersWithListener(*this, that);
			return *this;
		}

		EventListener& operator=(EventListener&& that) noexcept
		{
			EventSystem::get().removeHandlersWithListener(*this);
			EventSystem::get().replaceListener(*this, that);
			return *this;
		}

	protected:
		template<auto... Handlers> void registerEventHandlers()
		{
			(registerHandler<Handlers>(), ...);
		}

	private:
		template<typename> struct FunctionTraits;
		template<typename R, typename C, typename P> struct FunctionTraits<R (C::*)(P)>
		{
			using Return = R;
			using Class	 = C;
			using Param	 = P;
		};

		template<auto Handler> void registerHandler()
		{
			using HandlerTraits = FunctionTraits<decltype(Handler)>;
			using TReturn		= HandlerTraits::Return;
			using TClass		= HandlerTraits::Class;
			using TEvent		= HandlerTraits::Param;

			auto func = +[](EventListener& listener, Event& e) {
				auto& event	 = static_cast<TEvent&>(e);
				auto& object = static_cast<TClass&>(listener);
				if constexpr(std::is_same_v<TReturn, void>)
				{
					(object.*Handler)(event);
					return false;
				}
				else
					return (object.*Handler)(event);
			};
			EventSystem::get().submitHandler(func, this, typeid(TEvent));
		}
	};
}
