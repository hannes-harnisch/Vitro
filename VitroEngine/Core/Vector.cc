module;
#include "Trace/Assert.hh"

#include <cmath>
#include <format>
#include <type_traits>
export module Vitro.Core.Vector;

namespace vt
{
	template<typename, int> struct Vector;
	template<typename, int, int> struct Matrix;

	template<typename> struct IsScalar : std::true_type
	{};
	template<typename T, int D> struct IsScalar<Vector<T, D>> : std::false_type
	{};
	template<typename T, int R, int C> struct IsScalar<Matrix<T, R, C>> : std::false_type
	{};
	export template<typename T> concept Scalar = IsScalar<T>::value;

	template<typename T, int D> struct VectorData
	{
		T data[D];
	};

	template<typename T> struct VectorData<T, 2>
	{
		union
		{
			T data[2];
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
			T data[3];
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
			T data[4];
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

	export template<typename TFrom, typename TTo> concept LosslesslyConvertibleTo = requires(TFrom from)
	{
		TTo {from};
	};

	export template<typename T, int D> struct Vector : VectorData<T, D>
	{
		static constexpr size_t size() noexcept
		{
			return D;
		}

		template<Scalar S, int D2> constexpr operator Vector<S, D2>() const noexcept requires LosslesslyConvertibleTo<T, S>
		{
			return to<S, D2>();
		}

		template<Scalar S, int D2>
		explicit constexpr operator Vector<S, D2>() const noexcept requires(!LosslesslyConvertibleTo<T, S>)
		{
			return to<S, D2>();
		}

		constexpr T& operator[](size_t index) noexcept
		{
			return this->data[index];
		}

		constexpr T operator[](size_t index) const noexcept
		{
			return this->data[index];
		}

		template<Scalar S> constexpr bool operator==(Vector<S, D> that) const noexcept
		{
			return std::equal(std::begin(this->data), std::end(this->data), std::begin(that.data));
		}

		template<Scalar S> constexpr bool operator!=(Vector<S, D> that) const noexcept
		{
			return !operator==(that);
		}

		constexpr auto operator+() const noexcept
		{
			Vector<decltype(+this->data[0]), D> promoted;
			for(int i = 0; i < D; ++i)
				promoted.data[i] = +this->data[i];
			return promoted;
		}

		template<Scalar S> constexpr auto operator+(Vector<S, D> that) const noexcept
		{
			Vector<decltype(this->data[0] + that[0]), D> sum;
			for(int i = 0; i < D; ++i)
				sum.data[i] = this->data[i] + that.data[i];
			return sum;
		}

		constexpr auto operator+(Scalar auto scalar) const noexcept
		{
			Vector<decltype(this->data[0] + scalar), D> sum;
			for(int i = 0; i < D; ++i)
				sum.data[i] = this->data[i] + scalar;
			return sum;
		}

		friend constexpr auto operator+(Scalar auto scalar, Vector vec) noexcept
		{
			return vec + scalar;
		}

		template<Scalar S> constexpr Vector& operator+=(Vector<S, D> that) noexcept
		{
			for(int i = 0; i < D; ++i)
				this->data[i] += that.data[i];
			return *this;
		}

		constexpr Vector& operator+=(Scalar auto scalar) noexcept
		{
			for(T& component : this->data)
				component += scalar;
			return *this;
		}

		constexpr auto operator-() const noexcept
		{
			Vector<decltype(-this->data[0]), D> negated;
			for(int i = 0; i < D; ++i)
				negated.data[i] = -this->data[i];
			return negated;
		}

		template<Scalar S> constexpr auto operator-(Vector<S, D> that) const noexcept
		{
			Vector<decltype(this->data[0] - that[0]), D> difference;
			for(int i = 0; i < D; ++i)
				difference.data[i] = this->data[i] - that.data[i];
			return difference;
		}

		constexpr auto operator-(Scalar auto scalar) const noexcept
		{
			Vector<decltype(this->data[0] - scalar), D> difference;
			for(int i = 0; i < D; ++i)
				difference.data[i] = this->data[i] - scalar;
			return difference;
		}

		friend constexpr auto operator-(Scalar auto scalar, Vector vec) noexcept
		{
			Vector<decltype(scalar - vec[0]), D> difference;
			for(int i = 0; i < D; ++i)
				difference.data[i] = scalar - vec.data[i];
			return difference;
		}

		template<Scalar S> constexpr Vector& operator-=(Vector<S, D> that) noexcept
		{
			for(int i = 0; i < D; ++i)
				this->data[i] -= that.data[i];
			return *this;
		}

		constexpr Vector& operator-=(Scalar auto scalar) noexcept
		{
			for(T& component : this->data)
				component -= scalar;
			return *this;
		}

		template<Scalar S> constexpr auto operator*(Vector<S, D> that) const noexcept
		{
			Vector<decltype(this->data[0] * that[0]), D> product;
			for(int i = 0; i < D; ++i)
				product.data[i] = this->data[i] * that.data[i];
			return product;
		}

		constexpr auto operator*(Scalar auto scalar) const noexcept
		{
			Vector<decltype(this->data[0] * scalar), D> product;
			for(int i = 0; i < D; ++i)
				product.data[i] = this->data[i] * scalar;
			return product;
		}

		friend constexpr auto operator*(Scalar auto scalar, Vector vec) noexcept
		{
			return vec * scalar;
		}

		template<Scalar S> constexpr Vector& operator*=(Vector<S, D> that) noexcept
		{
			for(int i = 0; i < D; ++i)
				this->data[i] *= that.data[i];
			return *this;
		}

		constexpr Vector& operator*=(Scalar auto scalar) noexcept
		{
			for(T& component : this->data)
				component *= scalar;
			return *this;
		}

		template<Scalar S> constexpr auto operator/(Vector<S, D> that) const noexcept
		{
			Vector<decltype(this->data[0] / that[0]), D> quotient;
			for(int i = 0; i < D; ++i)
				quotient.data[i] = this->data[i] / that.data[i];
			return quotient;
		}

		constexpr auto operator/(Scalar auto scalar) const noexcept
		{
			Vector<decltype(this->data[0] / scalar), D> quotient;
			for(int i = 0; i < D; ++i)
				quotient.data[i] = this->data[i] / scalar;
			return quotient;
		}

		friend constexpr auto operator/(Scalar auto scalar, Vector vec) noexcept
		{
			Vector<decltype(scalar / vec[0]), D> quotient;
			for(int i = 0; i < D; ++i)
				quotient.data[i] = scalar / vec.data[i];
			return quotient;
		}

		template<Scalar S> constexpr Vector& operator/=(Vector<S, D> that) noexcept
		{
			for(int i = 0; i < D; ++i)
				this->data[i] /= that.data[i];
			return *this;
		}

		constexpr Vector& operator/=(Scalar auto scalar) noexcept
		{
			for(T& component : this->data)
				component /= scalar;
			return *this;
		}

		template<Scalar S, int D2> constexpr Vector<S, D2> to() const noexcept
		{
			Vector<S, D2> cast {};
			for(int i = 0; i < std::min(D, D2); ++i)
				cast[i] = static_cast<S>(this->data[i]);
			return cast;
		}

		std::string toString() const
		{
			auto str = std::format("[{}", this->data[0]);

			for(int i = 1; i < D; ++i)
				str += std::format(", {}", this->data[i]);

			return str + ']';
		}

		friend constexpr auto apply(Vector vec, auto func)
		{
			Vector<decltype(func(vec[0])), D> result;
			for(int i = 0; i < D; ++i)
				result.data[i] = func(vec.data[i]);
			return result;
		}

		template<Scalar S> friend constexpr auto dot(Vector left, Vector<S, D> right) noexcept
		{
			auto const hadamard = left * right;

			decltype(hadamard[0]) dot = 0;
			for(auto component : hadamard.data)
				dot += component;
			return dot;
		}

		friend constexpr auto length(Vector vec)
		{
			return std::sqrt(dot(vec, vec));
		}

		friend constexpr auto normalize(Vector vec)
		{
			return 1.0f / length(vec) * vec;
		}

		template<Scalar S> friend constexpr auto distance(Vector left, Vector<S, D> right)
		{
			return length(left - right);
		}

		friend constexpr auto sqrt(Vector vec)
		{
			return apply(vec, [](auto component) { return std::sqrt(component); });
		}

		friend constexpr auto invSqrt(Vector vec)
		{
			return 1.0f / sqrt(vec);
		}

		friend constexpr auto sin(Vector vec)
		{
			return apply(vec, [](auto component) { return std::sin(component); });
		}

		friend constexpr auto cos(Vector vec)
		{
			return apply(vec, [](auto component) { return std::cos(component); });
		}
	};

	export template<typename T, Scalar S>
	constexpr auto cross(Vector<T, 3> left, Vector<S, 3> right) noexcept -> Vector<decltype(left[0] * right[0]), 3>
	{
		return {left.y * right.z - right.y * left.z, left.z * right.x - right.z * left.x, left.x * right.y - right.x * left.y};
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