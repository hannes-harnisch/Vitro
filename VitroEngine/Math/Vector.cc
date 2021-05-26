module;
#include "Trace/Assert.hh"

#include <cmath>
#include <format>
#include <type_traits>
export module Vitro.Math.Vector;

template<typename, int> struct Vector;
template<typename, int, int> struct Matrix;

template<typename> struct IsScalar : std::true_type
{};
template<typename T, int D> struct IsScalar<Vector<T, D>> : std::false_type
{};
template<typename T, int M, int N> struct IsScalar<Matrix<T, M, N>> : std::false_type
{};
export template<typename T> concept Scalar = IsScalar<T>::value;

template<typename T, int D> struct VectorData
{
	T data[D] {};
};

template<typename T> struct VectorData<T, 2>
{
	union
	{
		T data[2] {};
		struct
		{
			T x, y;
		};
		struct
		{
			T r, g;
		};
	};
};

template<typename T> struct VectorData<T, 3>
{
	union
	{
		T data[3] {};
		struct
		{
			T x, y, z;
		};
		struct
		{
			T r, g, b;
		};
	};
};

template<typename T> struct VectorData<T, 4>
{
	union
	{
		T data[4] {};
		struct
		{
			T x, y, z, w;
		};
		struct
		{
			T r, g, b, a;
		};
	};
};

export template<typename From, typename To> concept LosslesslyConvertibleTo = requires(From from)
{
	To {from};
};

export template<typename T, int D> struct Vector : VectorData<T, D>
{
	template<Scalar S, int D2> constexpr operator Vector<S, D2>() const requires LosslesslyConvertibleTo<T, S>
	{
		return to<S, D2>();
	}

	template<Scalar S, int D2> explicit constexpr operator Vector<S, D2>() const requires(!LosslesslyConvertibleTo<T, S>)
	{
		return to<S, D2>();
	}

	constexpr T& operator[](size_t index)
	{
		return this->data[index];
	}

	constexpr T operator[](size_t index) const
	{
		return this->data[index];
	}

	template<Scalar S> constexpr bool operator==(const Vector<S, D>& that) const
	{
		return std::equal(std::begin(this->data), std::end(this->data), std::begin(that.data));
	}

	template<Scalar S> constexpr bool operator!=(const Vector<S, D>& that) const
	{
		return !operator==(that);
	}

	constexpr auto operator+() const
	{
		Vector<decltype(+this->data[0]), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = +this->data[i];
		return result;
	}

	template<Scalar S> constexpr auto operator+(const Vector<S, D>& that) const
	{
		Vector<decltype(this->data[0] + that[0]), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = this->data[i] + that.data[i];
		return result;
	}

	template<Scalar S> constexpr auto operator+(S scalar) const
	{
		Vector<decltype(this->data[0] + scalar), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = this->data[i] + scalar;
		return result;
	}

	template<Scalar S> friend constexpr auto operator+(S scalar, const Vector& vec)
	{
		return vec + scalar;
	}

	template<Scalar S> constexpr Vector& operator+=(const Vector<S, D>& that)
	{
		for(int i {}; i < D; ++i)
			this->data[i] += that.data[i];
		return *this;
	}

	template<Scalar S> constexpr Vector& operator+=(S scalar)
	{
		for(T& component : this->data)
			component += scalar;
		return *this;
	}

	constexpr auto operator-() const
	{
		Vector<decltype(-this->data[0]), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = -this->data[i];
		return result;
	}

	template<Scalar S> constexpr auto operator-(const Vector<S, D>& that) const
	{
		Vector<decltype(this->data[0] - that[0]), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = this->data[i] - that.data[i];
		return result;
	}

	template<Scalar S> constexpr auto operator-(S scalar) const
	{
		Vector<decltype(this->data[0] - scalar), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = this->data[i] - scalar;
		return result;
	}

	template<Scalar S> friend constexpr auto operator-(S scalar, const Vector& vec)
	{
		Vector<decltype(scalar - vec[0]), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = scalar - vec.data[i];
		return result;
	}

	template<Scalar S> constexpr Vector& operator-=(const Vector<S, D>& that)
	{
		for(int i {}; i < D; ++i)
			this->data[i] -= that.data[i];
		return *this;
	}

	template<Scalar S> constexpr Vector& operator-=(S scalar)
	{
		for(T& component : this->data)
			component -= scalar;
		return *this;
	}

	template<Scalar S> constexpr auto operator*(const Vector<S, D>& that) const
	{
		Vector<decltype(this->data[0] * that[0]), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = this->data[i] * that.data[i];
		return result;
	}

	template<Scalar S> constexpr auto operator*(S scalar) const
	{
		Vector<decltype(this->data[0] * scalar), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = this->data[i] * scalar;
		return result;
	}

	template<Scalar S> friend constexpr auto operator*(S scalar, const Vector& vec)
	{
		return vec * scalar;
	}

	template<Scalar S> constexpr Vector& operator*=(const Vector<S, D>& that)
	{
		for(int i {}; i < D; ++i)
			this->data[i] *= that.data[i];
		return *this;
	}

	template<Scalar S> constexpr Vector& operator*=(S scalar)
	{
		for(T& component : this->data)
			component *= scalar;
		return *this;
	}

	template<Scalar S> constexpr auto operator/(const Vector<S, D>& that) const
	{
		Vector<decltype(this->data[0] / that[0]), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = this->data[i] / that.data[i];
		return result;
	}

	template<Scalar S> constexpr auto operator/(S scalar) const
	{
		Vector<decltype(this->data[0] / scalar), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = this->data[i] / scalar;
		return result;
	}

	template<Scalar S> friend constexpr auto operator/(S scalar, const Vector& vec)
	{
		Vector<decltype(scalar / vec[0]), D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = scalar / vec.data[i];
		return result;
	}

	template<Scalar S> constexpr Vector& operator/=(const Vector<S, D>& that)
	{
		for(int i {}; i < D; ++i)
			this->data[i] /= that.data[i];
		return *this;
	}

	template<Scalar S> constexpr Vector& operator/=(S scalar)
	{
		for(T& component : this->data)
			component /= scalar;
		return *this;
	}

	constexpr size_t size() const
	{
		return D;
	}

	template<Scalar S, int D2> constexpr Vector<S, D2> to() const
	{
		Vector<S, D2> result;
		for(int i {}; i < std::min(D, D2); ++i)
			result[i] = static_cast<S>(this->data[i]);
		return result;
	}

	std::string toString() const
	{
		auto str = std::format("[{}", this->data[0]);

		for(int i = 1; i < D; ++i)
			str += std::format(", {}", this->data[i]);

		return str + "]";
	}

	template<typename R, Scalar S = T> friend constexpr Vector<R, D> apply(const Vector& vec, R (*func)(S))
	{
		Vector<R, D> result;
		for(int i {}; i < D; ++i)
			result.data[i] = func(vec.data[i]);
		return result;
	}

	template<Scalar S> friend constexpr auto dot(const Vector& left, const Vector<S, D>& right)
	{
		const auto hadamard {left * right};
		decltype(hadamard[0]) dot {};
		for(auto component : hadamard.data)
			dot += component;
		return dot;
	}

	friend constexpr auto length(const Vector& vec)
	{
		return std::sqrt(dot(vec, vec));
	}

	friend constexpr auto normalize(const Vector& vec)
	{
		return 1.0f / length(vec) * vec;
	}

	template<Scalar S> friend constexpr auto distance(const Vector& left, const Vector<S, D>& right)
	{
		return length(left - right);
	}

	friend constexpr auto sqrt(const Vector& vec)
	{
		return apply<decltype(std::sqrt(vec[0]))>(vec, std::sqrt);
	}

	friend constexpr auto invSqrt(const Vector& vec)
	{
		return 1.0f / sqrt(vec);
	}

	friend constexpr auto sin(const Vector& vec)
	{
		return apply<decltype(std::sin(vec[0]))>(vec, std::sin);
	}

	friend constexpr auto cos(const Vector& vec)
	{
		return apply<decltype(std::cos(vec[0]))>(vec, std::cos);
	}
};

export template<typename T, Scalar S>
constexpr auto cross(const Vector<T, 3>& left, const Vector<S, 3>& right) -> Vector<decltype(left[0] * right[0]), 3>
{
	return {left.y * right.z - right.y * left.z, left.z * right.x - right.z * left.x, left.x * right.y - right.x * left.y};
}

export using Int2	= Vector<int, 2>;
export using Int3	= Vector<int, 3>;
export using Int4	= Vector<int, 4>;
export using Float2 = Vector<float, 2>;
export using Float3 = Vector<float, 3>;
export using Float4 = Vector<float, 4>;
