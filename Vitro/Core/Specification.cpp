module;
#include <concepts>
#include <stdexcept>
#include <type_traits>
#include <utility>
export module vt.Core.Specification;

namespace vt
{
	// Wraps any unsigned integer type but prevents the value zero. To be used when zero is not an acceptable value in a
	// designated initializer.
	export template<std::unsigned_integral T> class Positive
	{
	public:
		Positive(T value) : value(value)
		{
			if(value == 0)
				throw std::invalid_argument("This value cannot be zero.");
		}

		operator T() const noexcept
		{
			return value;
		}

	private:
		T value;
	};

	// Forces explicit initialization with designated initializers. To be used when no reasonable default value can be used.
	export template<typename T> struct Explicit : T
	{
		Explicit() = delete;

		using T::T;
	};

	export template<typename T>
	requires std::is_scalar_v<T>
	struct Explicit<T>
	{
		Explicit(T value) noexcept : value(value)
		{}

		operator T() const noexcept
		{
			return value;
		}

	private:
		T value;
	};

	// Forces explicit initialization with designated initializers. To be used when no reasonable default value can be used.
	export template<typename T>
	requires std::is_union_v<T>
	struct Explicit<T>
	{
		Explicit() = delete;

		template<typename... Ts>
		Explicit(Ts&&... ts) noexcept(std::is_nothrow_constructible_v<T, Ts...>) : value {std::forward<Ts>(ts)...}
		{}

		operator T&() noexcept
		{
			return value;
		}

		operator T const &() const noexcept
		{
			return value;
		}

	private:
		T value;
	};
}
