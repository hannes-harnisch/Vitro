﻿module;
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
		Vector<To, TO_SIZE> cast;
		for(int i = 0; i != std::min(TO_SIZE, FROM_SIZE); ++i)
			cast[i] = static_cast<To>(vec.arr[i]);
		return cast;
	}

#define DEFINE_VECTOR_CONVERSIONS_AND_SUBSCRIPT(D)                                                                             \
	template<typename T2, int D2> constexpr operator Vector<T2, D2>() const noexcept requires LosslesslyConvertibleTo<T, T2>   \
	{                                                                                                                          \
		return vector_cast<T2, D2>(*this);                                                                                     \
	}                                                                                                                          \
                                                                                                                               \
	template<typename T2, int D2>                                                                                              \
	explicit constexpr operator Vector<T2, D2>() const noexcept requires(!LosslesslyConvertibleTo<T, T2>)                      \
	{                                                                                                                          \
		return vector_cast<T2, D2>(*this);                                                                                     \
	}                                                                                                                          \
                                                                                                                               \
	constexpr T& operator[](size_t index) noexcept                                                                             \
	{                                                                                                                          \
		return arr[index];                                                                                                     \
	}                                                                                                                          \
                                                                                                                               \
	constexpr T operator[](size_t index) const noexcept                                                                        \
	{                                                                                                                          \
		return arr[index];                                                                                                     \
	}                                                                                                                          \
                                                                                                                               \
	std::string to_string() const                                                                                              \
	{                                                                                                                          \
		auto str = std::format("[{}", arr[0]);                                                                                 \
                                                                                                                               \
		for(int i = 1; i != D; ++i)                                                                                            \
			str += std::format(", {}", arr[i]);                                                                                \
                                                                                                                               \
		return str + ']';                                                                                                      \
	}

	export template<typename T, int D> union Vector
	{
		T arr[D] = {};

		DEFINE_VECTOR_CONVERSIONS_AND_SUBSCRIPT(D)
	};

	export template<typename T> union Vector<T, 2>
	{
		T arr[2] = {};
		struct
		{
			T x, y;
		};
		struct
		{
			T r, g;
		};

		DEFINE_VECTOR_CONVERSIONS_AND_SUBSCRIPT(2)
	};

	export template<typename T> union Vector<T, 3>
	{
		T arr[3] = {};
		struct
		{
			T x, y, z;
		};
		struct
		{
			T r, g, b;
		};

		DEFINE_VECTOR_CONVERSIONS_AND_SUBSCRIPT(3)
	};

	export template<typename T> union Vector<T, 4>
	{
		T arr[4] = {};
		struct
		{
			T x, y, z, w;
		};
		struct
		{
			T r, g, b, a;
		};

		DEFINE_VECTOR_CONVERSIONS_AND_SUBSCRIPT(4)
	};

	export template<typename T1, typename T2, int D> constexpr bool operator==(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		return std::equal(std::begin(left.arr), std::end(left.arr), std::begin(right.arr));
	}

	export template<typename T1, typename T2, int D> constexpr bool operator!=(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		return !operator==(left, right);
	}

	export template<typename T, int D> constexpr auto operator+(Vector<T, D> vec) noexcept
	{
		Vector<decltype(+vec[0]), D> promoted;
		for(int i = 0; i != D; ++i)
			promoted.arr[i] = +vec.arr[i];
		return promoted;
	}

	export template<typename T1, typename T2, int D> constexpr auto operator+(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		Vector<decltype(left[0] + right[0]), D> sum;
		for(int i = 0; i != D; ++i)
			sum.arr[i] = left.arr[i] + right.arr[i];
		return sum;
	}

	export template<typename T, int D> constexpr auto operator+(Vector<T, D> vec, auto scalar) noexcept
	{
		Vector<decltype(vec[0] + scalar), D> sum;
		for(int i = 0; i != D; ++i)
			sum.arr[i] = vec.arr[i] + scalar;
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
			left.arr[i] += right.arr[i];
		return left;
	}

	export template<typename T, int D> constexpr Vector<T, D>& operator+=(Vector<T, D>& vec, auto scalar) noexcept
	{
		for(T& component : vec.arr)
			component += scalar;
		return vec;
	}

	export template<typename T, int D> constexpr auto operator-(Vector<T, D> vec) noexcept
	{
		Vector<decltype(-vec[0]), D> negated;
		for(int i = 0; i != D; ++i)
			negated.arr[i] = -vec.arr[i];
		return negated;
	}

	export template<typename T1, typename T2, int D> constexpr auto operator-(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		Vector<decltype(left[0] - right[0]), D> difference;
		for(int i = 0; i != D; ++i)
			difference.arr[i] = left.arr[i] - right.arr[i];
		return difference;
	}

	export template<typename T, int D> constexpr auto operator-(Vector<T, D> vec, auto scalar) noexcept
	{
		Vector<decltype(vec[0] - scalar), D> difference;
		for(int i = 0; i != D; ++i)
			difference.arr[i] = vec.arr[i] - scalar;
		return difference;
	}

	export template<typename T, int D> constexpr auto operator-(auto scalar, Vector<T, D> vec) noexcept
	{
		Vector<decltype(scalar - vec[0]), D> difference;
		for(int i = 0; i != D; ++i)
			difference.arr[i] = scalar - vec.arr[i];
		return difference;
	}

	export template<typename T1, typename T2, int D>
	constexpr Vector<T1, D>& operator-=(Vector<T1, D>& left, Vector<T2, D> right) noexcept
	{
		for(int i = 0; i != D; ++i)
			left.arr[i] -= right.arr[i];
		return left;
	}

	export template<typename T, int D> constexpr Vector<T, D>& operator-=(Vector<T, D>& vec, auto scalar) noexcept
	{
		for(T& component : vec.arr)
			component -= scalar;
		return vec;
	}

	export template<typename T1, typename T2, int D> constexpr auto operator*(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		Vector<decltype(left[0] * right[0]), D> product;
		for(int i = 0; i != D; ++i)
			product.arr[i] = left.arr[i] * right.arr[i];
		return product;
	}

	export template<typename T, int D> constexpr auto operator*(Vector<T, D> vec, auto scalar) noexcept
	{
		Vector<decltype(vec[0] * scalar), D> product;
		for(int i = 0; i != D; ++i)
			product.arr[i] = vec.arr[i] * scalar;
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
			left.arr[i] *= right.arr[i];
		return left;
	}

	export template<typename T, int D> constexpr Vector<T, D>& operator*=(Vector<T, D>& vec, auto scalar) noexcept
	{
		for(T& component : vec.arr)
			component *= scalar;
		return vec;
	}

	export template<typename T1, typename T2, int D> constexpr auto operator/(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		Vector<decltype(left[0] / right[0]), D> quotient;
		for(int i = 0; i != D; ++i)
			quotient.arr[i] = left.arr[i] / right.arr[i];
		return quotient;
	}

	export template<typename T, int D> constexpr auto operator/(Vector<T, D> vec, auto scalar) noexcept
	{
		Vector<decltype(vec[0] / scalar), D> quotient;
		for(int i = 0; i != D; ++i)
			quotient.arr[i] = vec.arr[i] / scalar;
		return quotient;
	}

	export template<typename T, int D> constexpr auto operator/(auto scalar, Vector<T, D> vec) noexcept
	{
		Vector<decltype(scalar / vec[0]), D> quotient;
		for(int i = 0; i != D; ++i)
			quotient.arr[i] = scalar / vec.arr[i];
		return quotient;
	}

	export template<typename T1, typename T2, int D>
	constexpr Vector<T1, D>& operator/=(Vector<T1, D>& left, Vector<T2, D> right) noexcept
	{
		for(int i = 0; i != D; ++i)
			left.arr[i] /= right.arr[i];
		return left;
	}

	export template<typename T, int D> constexpr Vector<T, D>& operator/=(Vector<T, D>& vec, auto scalar) noexcept
	{
		for(T& component : vec.arr)
			component /= scalar;
		return vec;
	}

	export template<typename T, int D> constexpr auto apply(Vector<T, D> vec, auto func) noexcept(noexcept(func(vec[0])))
	{
		Vector<decltype(func(vec[0])), D> result;
		for(int i = 0; i != D; ++i)
			result.arr[i] = func(vec.arr[i]);
		return result;
	}

	export template<typename T1, typename T2, int D> constexpr auto dot(Vector<T1, D> left, Vector<T2, D> right) noexcept
	{
		auto const hadamard = left * right;

		decltype(hadamard[0]) dot = 0;
		for(auto component : hadamard.arr)
			dot += component;
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
		return apply(vec, [](auto component) {
			return std::sqrt(component);
		});
	}

	export template<typename T, int D> constexpr auto inv_sqrt(Vector<T, D> vec)
	{
		return 1.0f / sqrt(vec);
	}

	export template<typename T, int D> constexpr auto sin(Vector<T, D> vec)
	{
		return apply(vec, [](auto component) {
			return std::sin(component);
		});
	}

	export template<typename T, int D> constexpr auto cos(Vector<T, D> vec)
	{
		return apply(vec, [](auto component) {
			return std::cos(component);
		});
	}

	export template<typename T, int D> constexpr auto abs(Vector<T, D> vec)
	{
		return apply(vec, [](auto component) {
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
