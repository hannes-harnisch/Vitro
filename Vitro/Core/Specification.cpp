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
		Positive(T value) : val(value)
		{
			if(val == 0)
				throw std::invalid_argument("This value is not allowed to be zero.");
		}

		operator T() const noexcept
		{
			return val;
		}

		T get() const noexcept
		{
			return val;
		}

	private:
		T val;
	};

	// When a data member is wrapped in this, it means there is no reasonable default value, so an explicit value must be
	// provided for it to be initialized.
	export template<typename T> struct Explicit : T
	{
		Explicit() = delete;

		using T::T;

		T& get() noexcept
		{
			return *this;
		}

		T const& get() const noexcept
		{
			return *this;
		}
	};

	// When a data member is wrapped in this, it means there is no reasonable default value, so an explicit value must be
	// provided for it to be initialized.
	export template<typename T>
	requires std::is_scalar_v<T>
	struct Explicit<T>
	{
		Explicit(T value) noexcept : val(value)
		{}

		operator T() const noexcept
		{
			return val;
		}

		T get() const noexcept
		{
			return val;
		}

	private:
		T val;
	};

	// When a data member is wrapped in this, it means there is no reasonable default value, so an explicit value must be
	// provided for it to be initialized.
	export template<typename T>
	requires(std::is_union_v<T> || std::is_final_v<T>) struct Explicit<T>
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
