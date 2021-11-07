module;
#include <type_traits>
export module vt.Core.Scope;

namespace vt
{
	export struct NoCopyNoMove
	{
		NoCopyNoMove() = default;

		NoCopyNoMove(NoCopyNoMove const&) = delete;
		NoCopyNoMove(NoCopyNoMove&&)	  = delete;
		NoCopyNoMove& operator=(NoCopyNoMove const&) = delete;
		NoCopyNoMove& operator=(NoCopyNoMove&&) = delete;
	};

	export template<typename Func> class ScopeExit : NoCopyNoMove
	{
	public:
		ScopeExit(Func func) noexcept(std::is_nothrow_move_constructible_v<Func>) : func(std::move(func))
		{}

		~ScopeExit() noexcept(std::is_nothrow_invocable_v<Func>)
		{
			func();
		}

	private:
		Func func;
	};

	export template<typename Func> class ScopeGuard : NoCopyNoMove
	{
	public:
		ScopeGuard(Func func) noexcept(std::is_nothrow_move_constructible_v<Func>) : func(std::move(func))
		{}

		~ScopeGuard() noexcept(std::is_nothrow_invocable_v<Func>)
		{
			if(!dismissed)
				func();
		}

		void dismiss()
		{
			dismissed = true;
		}

	private:
		Func func;
		bool dismissed = false;
	};
}
