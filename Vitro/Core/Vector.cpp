module;
#include <cmath>
#include <format>
#include <string>
#include <type_traits>
export module vt.Core.Vector;

namespace vt
{
	export template<typename From, typename To>
	concept LosslesslyConvertibleTo = requires(From from)
	{
		To {from};
	};

	template<typename T, int D> union Vector;

	export template<typename To, int TO_SIZE, typename From, int FROM_SIZE>
	constexpr Vector<To, TO_SIZE> vector_cast(Vector<From, FROM_SIZE> vec) noexcept
	{
		Vector<To, TO_SIZE> cast {};
		for(int i = 0; i != std::min(TO_SIZE, FROM_SIZE); ++i)
			cast[i] = static_cast<To>(vec[i]);
		return cast;
	}

#define DEFINE_VECTOR_CONVERSIONS(D)                                                                                           \
	template<typename T2, int D2> constexpr operator Vector<T2, D2>() const noexcept requires LosslesslyConvertibleTo<T, T2>   \
	{                                                                                                                          \
		return vector_cast<T2, D2>(*this);                                                                                     \
	}                                                                                                                          \
                                                                                                                               \
	template<typename T2, int D2>                                                                                              \
	explicit constexpr operator Vector<T2, D2>() const noexcept requires(!LosslesslyConvertibleTo<T, T2>)                      \
	{                                                                                                                          \
		return vector_cast<T2, D2>(*this);                                                                                     \
	}

	export template<typename T, int D> union Vector
	{
		T arr[D];

		constexpr T& operator[](size_t index) noexcept
		{
			return arr[index];
		}

		constexpr T const& operator[](size_t index) const noexcept
		{
			return arr[index];
		}

		std::string to_string() const
		{
			auto str = std::format("[{}", (*this)[0]);

			for(int i = 1; i != D; ++i)
				str += std::format(", {}", (*this)[i]);

			return str + ']';
		}

		DEFINE_VECTOR_CONVERSIONS(D)
	};

	export template<typename T> union Vector<T, 2>
	{
		struct
		{
			T x, y;
		};
		struct
		{
			T r, g;
		};

		constexpr T& operator[](size_t index) noexcept
		{
			return this->*MEMBERS[index];
		}

		constexpr T const& operator[](size_t index) const noexcept
		{
			return this->*MEMBERS[index];
		}

		std::string to_string() const
		{
			return std::format("[{}, {}]", x, y);
		}

		DEFINE_VECTOR_CONVERSIONS(2)

	private:
		static constexpr T Vector::*MEMBERS[] {&Vector::x, &Vector::y};
	};

	export template<typename T> union Vector<T, 3>
	{
		struct
		{
			T x, y, z;
		};
		struct
		{
			T r, g, b;
		};

		constexpr T& operator[](size_t index) noexcept
		{
			return this->*MEMBERS[index];
		}

		constexpr T const& operator[](size_t index) const noexcept
		{
			return this->*MEMBERS[index];
		}

		std::string to_string() const
		{
			return std::format("[{}, {}, {}]", x, y, z);
		}

		DEFINE_VECTOR_CONVERSIONS(3)

	private:
		static constexpr T Vector::*MEMBERS[] {&Vector::x, &Vector::y, &Vector::z};
	};

	export template<typename T> union Vector<T, 4>
	{
		struct
		{
			T x, y, z, w;
		};
		struct
		{
			T r, g, b, a;
		};

		constexpr T& operator[](size_t index) noexcept
		{
			return this->*MEMBERS[index];
		}

		constexpr T const& operator[](size_t index) const noexcept
		{
			return this->*MEMBERS[index];
		}

		std::string to_string() const
		{
			return std::format("[{}, {}, {}, {}]", x, y, z, w);
		}

		DEFINE_VECTOR_CONVERSIONS(4)

	private:
		static constexpr T Vector::*MEMBERS[] {&Vector::x, &Vector::y, &Vector::z, &Vector::w};
	};

	export using Int2	= Vector<int, 2>;
	export using Int3	= Vector<int, 3>;
	export using Int4	= Vector<int, 4>;
	export using UInt2	= Vector<unsigned, 2>;
	export using UInt3	= Vector<unsigned, 3>;
	export using UInt4	= Vector<unsigned, 4>;
	export using Float2 = Vector<float, 2>;
	export using Float3 = Vector<float, 3>;
	export using Float4 = Vector<float, 4>;

	export template<typename T1, typename T2, int D> constexpr bool operator==(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		for(int i = 0; i != D; ++i)
			if(left[i] != right[i])
				return false;

		return true;
	}

	export template<typename T1, typename T2, int D> constexpr bool operator!=(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		return !operator==(left, right);
	}

	export template<typename T, int D> constexpr auto operator+(Vector<T, D> vec) noexcept
	{
		Vector<decltype(+vec[0]), D> promoted;
		for(int i = 0; i != D; ++i)
			promoted[i] = +vec[i];
		return promoted;
	}

	export template<typename T1, typename T2, int D> constexpr auto operator+(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		Vector<decltype(left[0] + right[0]), D> sum;
		for(int i = 0; i != D; ++i)
			sum[i] = left[i] + right[i];
		return sum;
	}

	export template<typename T, int D> constexpr auto operator+(Vector<T, D> vec, auto scalar) noexcept
	{
		Vector<decltype(vec[0] + scalar), D> sum;
		for(int i = 0; i != D; ++i)
			sum[i] = vec[i] + scalar;
		return sum;
	}

	export template<typename T, int D> constexpr auto operator+(auto scalar, Vector<T, D> vec) noexcept
	{
		return vec + scalar;
	}

	export template<typename T1, typename T2, int D>
	constexpr Vector<T1, D>& operator+=(Vector<T1, D>& left, Vector<T2, D> right) noexcept
	{
		for(int i = 0; i != D; ++i)
			left[i] += right[i];
		return left;
	}

	export template<typename T, int D> constexpr Vector<T, D>& operator+=(Vector<T, D>& vec, auto scalar) noexcept
	{
		for(T& component : vec)
			component += scalar;
		return vec;
	}

	export template<typename T, int D> constexpr auto operator-(Vector<T, D> vec) noexcept
	{
		Vector<decltype(-vec[0]), D> negated;
		for(int i = 0; i != D; ++i)
			negated[i] = -vec[i];
		return negated;
	}

	export template<typename T1, typename T2, int D> constexpr auto operator-(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		Vector<decltype(left[0] - right[0]), D> difference;
		for(int i = 0; i != D; ++i)
			difference[i] = left[i] - right[i];
		return difference;
	}

	export template<typename T, int D> constexpr auto operator-(Vector<T, D> vec, auto scalar) noexcept
	{
		Vector<decltype(vec[0] - scalar), D> difference;
		for(int i = 0; i != D; ++i)
			difference[i] = vec[i] - scalar;
		return difference;
	}

	export template<typename T, int D> constexpr auto operator-(auto scalar, Vector<T, D> vec) noexcept
	{
		Vector<decltype(scalar - vec[0]), D> difference;
		for(int i = 0; i != D; ++i)
			difference[i] = scalar - vec[i];
		return difference;
	}

	export template<typename T1, typename T2, int D>
	constexpr Vector<T1, D>& operator-=(Vector<T1, D>& left, Vector<T2, D> right) noexcept
	{
		for(int i = 0; i != D; ++i)
			left[i] -= right[i];
		return left;
	}

	export template<typename T, int D> constexpr Vector<T, D>& operator-=(Vector<T, D>& vec, auto scalar) noexcept
	{
		for(T& component : vec)
			component -= scalar;
		return vec;
	}

	export template<typename T1, typename T2, int D> constexpr auto operator*(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		Vector<decltype(left[0] * right[0]), D> product;
		for(int i = 0; i != D; ++i)
			product[i] = left[i] * right[i];
		return product;
	}

	export template<typename T, int D> constexpr auto operator*(Vector<T, D> vec, auto scalar) noexcept
	{
		Vector<decltype(vec[0] * scalar), D> product;
		for(int i = 0; i != D; ++i)
			product[i] = vec[i] * scalar;
		return product;
	}

	export template<typename T, int D> constexpr auto operator*(auto scalar, Vector<T, D> vec) noexcept
	{
		return vec * scalar;
	}

	export template<typename T1, typename T2, int D>
	constexpr Vector<T1, D>& operator*=(Vector<T1, D>& left, Vector<T2, D> right) noexcept
	{
		for(int i = 0; i != D; ++i)
			left[i] *= right[i];
		return left;
	}

	export template<typename T, int D> constexpr Vector<T, D>& operator*=(Vector<T, D>& vec, auto scalar) noexcept
	{
		for(T& component : vec)
			component *= scalar;
		return vec;
	}

	export template<typename T1, typename T2, int D> constexpr auto operator/(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		Vector<decltype(left[0] / right[0]), D> quotient;
		for(int i = 0; i != D; ++i)
			quotient[i] = left[i] / right[i];
		return quotient;
	}

	export template<typename T, int D> constexpr auto operator/(Vector<T, D> vec, auto scalar) noexcept
	{
		Vector<decltype(vec[0] / scalar), D> quotient;
		for(int i = 0; i != D; ++i)
			quotient[i] = vec[i] / scalar;
		return quotient;
	}

	export template<typename T, int D> constexpr auto operator/(auto scalar, Vector<T, D> vec) noexcept
	{
		Vector<decltype(scalar / vec[0]), D> quotient;
		for(int i = 0; i != D; ++i)
			quotient[i] = scalar / vec[i];
		return quotient;
	}

	export template<typename T1, typename T2, int D>
	constexpr Vector<T1, D>& operator/=(Vector<T1, D>& left, Vector<T2, D> right) noexcept
	{
		for(int i = 0; i != D; ++i)
			left[i] /= right[i];
		return left;
	}

	export template<typename T, int D> constexpr Vector<T, D>& operator/=(Vector<T, D>& vec, auto scalar) noexcept
	{
		for(T& component : vec)
			component /= scalar;
		return vec;
	}

	export template<typename T, int D> constexpr auto apply(Vector<T, D> vec, auto func) noexcept(noexcept(func(vec[0])))
	{
		Vector<decltype(func(vec[0])), D> result;
		for(int i = 0; i != D; ++i)
			result[i] = func(vec[i]);
		return result;
	}

	export template<typename T1, typename T2, int D> constexpr auto dot(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		auto const hadamard = left * right;

		decltype(left[0] * right[0]) dot = 0;
		for(int i = 0; i != D; ++i)
			dot += hadamard[i];
		return dot;
	}

	export template<typename T, int D> constexpr auto length(Vector<T, D> vec)
	{
		return std::sqrt(dot(vec, vec));
	}

	export template<typename T, int D> constexpr auto normalize(Vector<T, D> vec)
	{
		return 1.0f / length(vec) * vec;
	}

	export template<typename T1, typename T2, int D> constexpr auto distance(Vector<T1, D> left, Vector<T2, D> right)
	{
		return length(left - right);
	}

	export template<typename T, int D> constexpr auto sqrt(Vector<T, D> vec)
	{
		return apply(vec, [](T component) {
			return std::sqrt(component);
		});
	}

	export template<typename T, int D> constexpr auto inv_sqrt(Vector<T, D> vec)
	{
		return 1.0f / sqrt(vec);
	}

	export template<typename T, int D> constexpr auto sin(Vector<T, D> vec)
	{
		return apply(vec, [](T component) {
			return std::sin(component);
		});
	}

	export template<typename T, int D> constexpr auto cos(Vector<T, D> vec)
	{
		return apply(vec, [](T component) {
			return std::cos(component);
		});
	}

	export template<typename T, int D> constexpr auto abs(Vector<T, D> vec)
	{
		return apply(vec, [](T component) {
			return std::abs(component);
		});
	}

	export template<typename T1, typename T2>
	constexpr auto cross(Vector<T1, 3> left, Vector<T2, 3> right) noexcept -> Vector<decltype(left[0] * right[0]), 3>
	{
		return {
			left.y * right.z - right.y * left.z,
			left.z * right.x - right.z * left.x,
			left.x * right.y - right.x * left.y,
		};
	}

	export using Int2	= Vector<int, 2>;
	export using Int3	= Vector<int, 3>;
	export using Int4	= Vector<int, 4>;
	export using UInt2	= Vector<unsigned, 2>;
	export using UInt3	= Vector<unsigned, 3>;
	export using UInt4	= Vector<unsigned, 4>;
	export using Float2 = Vector<float, 2>;
	export using Float3 = Vector<float, 3>;
	export using Float4 = Vector<float, 4>;
}
