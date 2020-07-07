#pragma once

#include "_pch.h"
#include "Vitro/Utility/Assert.h"

#define VTR_IS_SCALAR(O) typename = typename std::enable_if_t<std::is_arithmetic_v<O>, O>

namespace Vitro
{
	template<size_t D, typename N> struct Vector;

	template<typename N> struct Vector<3, N> final
	{
		union
		{
			struct
			{
				N X, Y, Z;
			};
			struct
			{
				N R, G, B;
			};
			struct
			{
				N S, T, P;
			};
			N Raw[3];
		};

		template<typename O0, typename O1, typename O2>
		constexpr Vector(O0 x, O1 y, O2 z) : X(static_cast<N>(x)), Y(static_cast<N>(y)), Z(static_cast<N>(z))
		{}

		template<typename O>
		constexpr Vector(O scalar) : Vector(static_cast<N>(scalar), static_cast<N>(scalar), static_cast<N>(scalar))
		{}

		template<typename O> constexpr Vector(const Vector<2, O>& other) : Vector(other.X, other.Y, 0)
		{}

		template<size_t D, typename O> constexpr Vector(const Vector<D, O>& other) : Vector(other.X, other.Y, other.Z)
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

		template<typename O> constexpr Vector<3, N> operator+(const Vector<3, O>& v) const
		{
			return {X + v.X, Y + v.Y, Z + v.Z};
		}

		template<typename O> constexpr Vector<3, N> operator-(const Vector<3, O>& v) const
		{
			return {X - v.X, Y - v.Y, Z - v.Z};
		}

		template<typename O> constexpr Vector<3, N> operator*(const Vector<3, O>& v) const
		{
			return {X * v.X, Y * v.Y, Z * v.Z};
		}

		template<typename O> constexpr Vector<3, N> operator/(const Vector<3, O>& v) const
		{
			return {X / v.X, Y / v.Y, Z / v.Z};
		}

		template<typename O> constexpr Vector<3, N> operator%(const Vector<3, O>& v) const
		{
			return {X % v.X, Y % v.Y, Z % v.Z};
		}

		template<typename O> constexpr Vector<3, N> operator&(const Vector<3, O>& v) const
		{
			return {X & v.X, Y & v.Y, Z & v.Z};
		}

		template<typename O> constexpr Vector<3, N> operator|(const Vector<3, O>& v) const
		{
			return {X | v.X, Y | v.Y, Z | v.Z};
		}

		template<typename O> constexpr Vector<3, N> operator^(const Vector<3, O>& v) const
		{
			return {X ^ v.X, Y ^ v.Y, Z ^ v.Z};
		}

		template<typename O> constexpr Vector<3, N> operator<<(const Vector<3, O>& v) const
		{
			return {X << v.X, Y << v.Y, Z << v.Z};
		}

		template<typename O> constexpr Vector<3, N> operator>>(const Vector<3, O>& v) const
		{
			return {X >> v.X, Y >> v.Y, Z >> v.Z};
		}

		template<typename O> constexpr Vector<3, N>& operator+=(const Vector<3, O>& v)
		{
			X += v.X;
			Y += v.Y;
			Z += v.Z;
			return *this;
		}

		template<typename O> constexpr Vector<3, N>& operator-=(const Vector<3, O>& v)
		{
			X -= v.X;
			Y -= v.Y;
			Z -= v.Z;
			return *this;
		}

		template<typename O> constexpr Vector<3, N>& operator*=(const Vector<3, O>& v)
		{
			X *= v.X;
			Y *= v.Y;
			Z *= v.Z;
			return *this;
		}

		template<typename O> constexpr Vector<3, N>& operator/=(const Vector<3, O>& v)
		{
			X /= v.X;
			Y /= v.Y;
			Z /= v.Z;
			return *this;
		}

		template<typename O> constexpr Vector<3, N>& operator%=(const Vector<3, O>& v)
		{
			X %= v.X;
			Y %= v.Y;
			Z %= v.Z;
			return *this;
		}

		template<typename O> constexpr Vector<3, N>& operator&=(const Vector<3, O>& v)
		{
			X &= v.X;
			Y &= v.Y;
			Z &= v.Z;
			return *this;
		}

		template<typename O> constexpr Vector<3, N>& operator|=(const Vector<3, O>& v)
		{
			X |= v.X;
			Y |= v.Y;
			Z |= v.Z;
			return *this;
		}

		template<typename O> constexpr Vector<3, N>& operator^=(const Vector<3, O>& v)
		{
			X ^= v.X;
			Y ^= v.Y;
			Z ^= v.Z;
			return *this;
		}

		template<typename O> constexpr Vector<3, N>& operator<<=(const Vector<3, O>& v)
		{
			X <<= v.X;
			Y <<= v.Y;
			Z <<= v.Z;
			return *this;
		}

		template<typename O> constexpr Vector<3, N>& operator>>=(const Vector<3, O>& v)
		{
			X >>= v.X;
			Y >>= v.Y;
			Z >>= v.Z;
			return *this;
		}

		template<typename O> constexpr bool operator==(const Vector<3, O>& v) const
		{
			return X == v.X && Y == v.Y && Z == v.Z;
		}

		template<typename O> constexpr bool operator!=(const Vector<3, O>& v) const
		{
			return X != v.X || Y != v.Y || Z != v.Z;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator+(O scalar) const
		{
			return {X + scalar, Y + scalar, Z + scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator-(O scalar) const
		{
			return {X - scalar, Y - scalar, Z - scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator*(O scalar) const
		{
			return {X * scalar, Y * scalar, Z * scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator/(O scalar) const
		{
			return {X / scalar, Y / scalar, Z / scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator%(O scalar) const
		{
			return {X % scalar, Y % scalar, Z % scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator&(O scalar) const
		{
			return {X & scalar, Y & scalar, Z & scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator|(O scalar) const
		{
			return {X | scalar, Y | scalar, Z | scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator^(O scalar) const
		{
			return {X ^ scalar, Y ^ scalar, Z ^ scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator<<(O scalar) const
		{
			return {X << scalar, Y << scalar, Z << scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator>>(O scalar) const
		{
			return {X >> scalar, Y >> scalar, Z >> scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator+=(O scalar)
		{
			X += scalar;
			Y += scalar;
			Z += scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator-=(O scalar)
		{
			X -= scalar;
			Y -= scalar;
			Z -= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator*=(O scalar)
		{
			X *= scalar;
			Y *= scalar;
			Z *= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator/=(O scalar)
		{
			X /= scalar;
			Y /= scalar;
			Z /= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator%=(O scalar)
		{
			X %= scalar;
			Y %= scalar;
			Z %= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator&=(O scalar)
		{
			X &= scalar;
			Y &= scalar;
			Z &= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator|=(O scalar)
		{
			X |= scalar;
			Y |= scalar;
			Z |= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator^=(O scalar)
		{
			X ^= scalar;
			Y ^= scalar;
			Z ^= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator<<=(O scalar)
		{
			X <<= scalar;
			Y <<= scalar;
			Z <<= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N>& operator>>=(O scalar)
		{
			X >>= scalar;
			Y >>= scalar;
			Z >>= scalar;
			return *this;
		}

		constexpr Vector<3, N> operator+() const
		{
			return {+X, +Y, +Z};
		}

		constexpr Vector<3, N> operator-() const
		{
			return {-X, -Y, -Z};
		}

		constexpr Vector<3, N> operator~() const
		{
			return {~X, ~Y, ~Z};
		}

		constexpr Vector<3, N>& operator++()
		{
			++X;
			++Y;
			++Z;
			return *this;
		}

		constexpr Vector<3, N>& operator--()
		{
			--X;
			--Y;
			--Z;
			return *this;
		}

		constexpr Vector<3, N> operator++(int)
		{
			auto oldValue = *this;
			++X;
			++Y;
			++Z;
			return oldValue;
		}

		constexpr Vector<3, N> operator--(int)
		{
			auto oldValue = *this;
			--X;
			--Y;
			--Z;
			return oldValue;
		}

		std::string ToString() const
		{
			std::stringstream s;
			s << '(' << +X << ',' << +Y << ',' << +Z << ')';
			return s.str();
		}
	};

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator+(O scalar, const Vector<3, N>& v)
	{
		return {scalar + v.X, scalar + v.Y, scalar + v.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator-(O scalar, const Vector<3, N>& v)
	{
		return {scalar - v.X, scalar - v.Y, scalar - v.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator*(O scalar, const Vector<3, N>& v)
	{
		return {scalar * v.X, scalar * v.Y, scalar * v.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator/(O scalar, const Vector<3, N>& v)
	{
		return {scalar / v.X, scalar / v.Y, scalar / v.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator%(O scalar, const Vector<3, N>& v)
	{
		return {scalar % v.X, scalar % v.Y, scalar % v.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator&(O scalar, const Vector<3, N>& v)
	{
		return {scalar & v.X, scalar & v.Y, scalar & v.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator|(O scalar, const Vector<3, N>& v)
	{
		return {scalar | v.X, scalar | v.Y, scalar | v.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)> constexpr Vector<3, N> operator^(O scalar, const Vector<3, N>& v)
	{
		return {scalar ^ v.X, scalar ^ v.Y, scalar ^ v.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Vector<3, N> operator<<(O scalar, const Vector<3, N>& v)
	{
		return {scalar << v.X, scalar << v.Y, scalar << v.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Vector<3, N> operator>>(O scalar, const Vector<3, N>& v)
	{
		return {scalar >> v.X, scalar >> v.Y, scalar >> v.Z};
	}

	template<typename N, typename O> constexpr N Dot(const Vector<3, N>& lv, const Vector<3, O>& rv)
	{
		auto temp = lv * rv;
		return temp.X + temp.Y + temp.Z;
	}

	template<typename N, typename O> constexpr Vector<3, N> Cross(const Vector<3, N>& lv, const Vector<3, O>& rv)
	{
		return {lv.Y * rv.Z - rv.Y * lv.Z, lv.Z * rv.X - rv.Z * lv.X, lv.X * rv.Y - rv.X * lv.Y};
	}

	template<typename N, typename O> Vector<3, O> Apply(const Vector<3, N>& v, O (*func)(N))
	{
		return {func(v.X), func(v.Y), func(v.Z)};
	}
}
