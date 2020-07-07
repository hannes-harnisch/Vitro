#pragma once

#include "_pch.h"
#include "Vitro/Utility/Assert.h"

#define VTR_IS_SCALAR(O) typename = typename std::enable_if_t<std::is_arithmetic_v<O>, O>

namespace Vitro
{
	template<size_t D, typename N> struct Vector;

	template<typename N> struct Vector<2, N> final
	{
		union
		{
			struct
			{
				N X, Y;
			};
			struct
			{
				N R, G;
			};
			struct
			{
				N S, T;
			};
			N Raw[2];
		};

		template<typename O0, typename O1> constexpr Vector(O0 x, O1 y) : X(static_cast<N>(x)), Y(static_cast<N>(y))
		{}

		template<typename O> constexpr Vector(O scalar) : Vector(static_cast<N>(scalar), static_cast<N>(scalar))
		{}

		template<size_t D, typename O> constexpr Vector(const Vector<D, O>& other) : Vector(other.X, other.Y)
		{}

		constexpr Vector() : Vector(0)
		{}

		constexpr N& operator[](size_t index)
		{
			AssertArrayRange(Raw, index);
			return Raw[index];
		}

		constexpr const N& operator[](size_t index) const
		{
			AssertArrayRange(Raw, index);
			return Raw[index];
		}

		template<typename O> constexpr Vector<2, N> operator+(const Vector<2, O>& v) const
		{
			return {X + v.X, Y + v.Y};
		}

		template<typename O> constexpr Vector<2, N> operator-(const Vector<2, O>& v) const
		{
			return {X - v.X, Y - v.Y};
		}

		template<typename O> constexpr Vector<2, N> operator*(const Vector<2, O>& v) const
		{
			return {X * v.X, Y * v.Y};
		}

		template<typename O> constexpr Vector<2, N> operator/(const Vector<2, O>& v) const
		{
			return {X / v.X, Y / v.Y};
		}

		template<typename O> constexpr Vector<2, N> operator%(const Vector<2, O>& v) const
		{
			return {X % v.X, Y % v.Y};
		}

		template<typename O> constexpr Vector<2, N> operator&(const Vector<2, O>& v) const
		{
			return {X & v.X, Y & v.Y};
		}

		template<typename O> constexpr Vector<2, N> operator|(const Vector<2, O>& v) const
		{
			return {X | v.X, Y | v.Y};
		}

		template<typename O> constexpr Vector<2, N> operator^(const Vector<2, O>& v) const
		{
			return {X ^ v.X, Y ^ v.Y};
		}

		template<typename O> constexpr Vector<2, N> operator<<(const Vector<2, O>& v) const
		{
			return {X << v.X, Y << v.Y};
		}

		template<typename O> constexpr Vector<2, N> operator>>(const Vector<2, O>& v) const
		{
			return {X >> v.X, Y >> v.Y};
		}

		template<typename O> constexpr Vector<2, N>& operator+=(const Vector<2, O>& v)
		{
			X += v.X;
			Y += v.Y;
			return *this;
		}

		template<typename O> constexpr Vector<2, N>& operator-=(const Vector<2, O>& v)
		{
			X -= v.X;
			Y -= v.Y;
			return *this;
		}

		template<typename O> constexpr Vector<2, N>& operator*=(const Vector<2, O>& v)
		{
			X *= v.X;
			Y *= v.Y;
			return *this;
		}

		template<typename O> constexpr Vector<2, N>& operator/=(const Vector<2, O>& v)
		{
			X /= v.X;
			Y /= v.Y;
			return *this;
		}

		template<typename O> constexpr Vector<2, N>& operator%=(const Vector<2, O>& v)
		{
			X %= v.X;
			Y %= v.Y;
			return *this;
		}

		template<typename O> constexpr Vector<2, N>& operator&=(const Vector<2, O>& v)
		{
			X &= v.X;
			Y &= v.Y;
			return *this;
		}

		template<typename O> constexpr Vector<2, N>& operator|=(const Vector<2, O>& v)
		{
			X |= v.X;
			Y |= v.Y;
			return *this;
		}

		template<typename O> constexpr Vector<2, N>& operator^=(const Vector<2, O>& v)
		{
			X ^= v.X;
			Y ^= v.Y;
			return *this;
		}

		template<typename O> constexpr Vector<2, N>& operator<<=(const Vector<2, O>& v)
		{
			X <<= v.X;
			Y <<= v.Y;
			return *this;
		}

		template<typename O> constexpr Vector<2, N>& operator>>=(const Vector<2, O>& v)
		{
			X >>= v.X;
			Y >>= v.Y;
			return *this;
		}

		template<typename O> constexpr bool operator==(const Vector<2, O>& v) const
		{
			return X == v.X && Y == v.Y;
		}

		template<typename O> constexpr bool operator!=(const Vector<2, O>& v) const
		{
			return X != v.X || Y != v.Y;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator+(O scalar) const
		{
			return {X + scalar, Y + scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator-(O scalar) const
		{
			return {X - scalar, Y - scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator*(O scalar) const
		{
			return {X * scalar, Y * scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator/(O scalar) const
		{
			return {X / scalar, Y / scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator%(O scalar) const
		{
			return {X % scalar, Y % scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator&(O scalar) const
		{
			return {X & scalar, Y & scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator|(O scalar) const
		{
			return {X | scalar, Y | scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator^(O scalar) const
		{
			return {X ^ scalar, Y ^ scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator<<(O scalar) const
		{
			return {X << scalar, Y << scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator>>(O scalar) const
		{
			return {X >> scalar, Y >> scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator+=(O scalar)
		{
			X += scalar;
			Y += scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator-=(O scalar)
		{
			X -= scalar;
			Y -= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator*=(O scalar)
		{
			X *= scalar;
			Y *= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator/=(O scalar)
		{
			X /= scalar;
			Y /= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator%=(O scalar)
		{
			X %= scalar;
			Y %= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator&=(O scalar)
		{
			X &= scalar;
			Y &= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator|=(O scalar)
		{
			X |= scalar;
			Y |= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator^=(O scalar)
		{
			X ^= scalar;
			Y ^= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator<<=(O scalar)
		{
			X <<= scalar;
			Y <<= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N>& operator>>=(O scalar)
		{
			X >>= scalar;
			Y >>= scalar;
			return *this;
		}

		constexpr Vector<2, N> operator+() const
		{
			return {+X, +Y};
		}

		constexpr Vector<2, N> operator-() const
		{
			return {-X, -Y};
		}

		constexpr Vector<2, N> operator~() const
		{
			return {~X, ~Y};
		}

		constexpr Vector<2, N>& operator++()
		{
			++X;
			++Y;
			return *this;
		}

		constexpr Vector<2, N>& operator--()
		{
			--X;
			--Y;
			return *this;
		}

		constexpr Vector<2, N> operator++(int)
		{
			auto oldValue = *this;
			++X;
			++Y;
			return oldValue;
		}

		constexpr Vector<2, N> operator--(int)
		{
			auto oldValue = *this;
			--X;
			--Y;
			return oldValue;
		}

		std::string ToString() const
		{
			std::stringstream s;
			s << '(' << +X << ',' << +Y << ')';
			return s.str();
		}
	};

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator+(O scalar, const Vector<2, N>& v)
	{
		return {scalar + v.X, scalar + v.Y};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator-(O scalar, const Vector<2, N>& v)
	{
		return {scalar - v.X, scalar - v.Y};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator*(O scalar, const Vector<2, N>& v)
	{
		return {scalar * v.X, scalar * v.Y};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator/(O scalar, const Vector<2, N>& v)
	{
		return {scalar / v.X, scalar / v.Y};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator%(O scalar, const Vector<2, N>& v)
	{
		return {scalar % v.X, scalar % v.Y};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator&(O scalar, const Vector<2, N>& v)
	{
		return {scalar & v.X, scalar & v.Y};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator|(O scalar, const Vector<2, N>& v)
	{
		return {scalar | v.X, scalar | v.Y};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<2, N> operator^(O scalar, const Vector<2, N>& v)
	{
		return {scalar ^ v.X, scalar ^ v.Y};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Vector<2, N> operator<<(O scalar, const Vector<2, N>& v)
	{
		return {scalar << v.X, scalar << v.Y};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Vector<2, N> operator>>(O scalar, const Vector<2, N>& v)
	{
		return {scalar >> v.X, scalar >> v.Y};
	}

	template<typename N, typename O> constexpr N Dot(const Vector<2, N>& lv, const Vector<2, O>& rv)
	{
		auto temp = lv * rv;
		return temp.X + temp.Y;
	}

	template<typename N, typename O> Vector<2, O> Apply(const Vector<2, N>& v, O (*func)(N))
	{
		return {func(v.X), func(v.Y)};
	}
}
