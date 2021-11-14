module;
#include <cstddef>
#include <type_traits>
export module vt.Core.PointerProxy;

namespace vt
{
	// Use this class to use non-pointer types in smart pointer templates such as std::unique_ptr with a custom deleter. It's
	// sufficient to use this template as the pointer alias in the custom deleter, everything else will be deduced. Ensure that
	// operator== is overloaded for H.
	export template<typename H> struct PointerProxy : H
	{
		PointerProxy() : H()
		{}

		PointerProxy(std::nullptr_t) : H()
		{}

		PointerProxy(H handle) : H(handle)
		{}

		explicit operator bool() const
		{
			return *this != H();
		}
	};

	export template<typename H>
	requires std::is_fundamental_v<H>
	struct PointerProxy<H>
	{
		PointerProxy() = default;

		PointerProxy(std::nullptr_t)
		{}

		PointerProxy(H handle) : handle(handle)
		{}

		explicit operator bool() const
		{
			return handle != H();
		}

	private:
		H handle = {};
	};
}
