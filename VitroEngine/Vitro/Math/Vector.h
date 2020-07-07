#pragma once

#include "Vitro/Math/Vector2.h"
#include "Vitro/Math/Vector3.h"
#include "Vitro/Math/Vector4.h"

namespace Vitro
{
	typedef Vector<2, signed char> SByte2;
	typedef Vector<3, signed char> SByte3;
	typedef Vector<4, signed char> SByte4;
	typedef Vector<2, unsigned char> Byte2;
	typedef Vector<3, unsigned char> Byte3;
	typedef Vector<4, unsigned char> Byte4;
	typedef Vector<2, short> Short2;
	typedef Vector<3, short> Short3;
	typedef Vector<4, short> Short4;
	typedef Vector<2, unsigned short> UShort2;
	typedef Vector<3, unsigned short> UShort3;
	typedef Vector<4, unsigned short> UShort4;
	typedef Vector<2, int> Int2;
	typedef Vector<3, int> Int3;
	typedef Vector<4, int> Int4;
	typedef Vector<2, unsigned int> UInt2;
	typedef Vector<3, unsigned int> UInt3;
	typedef Vector<4, unsigned int> UInt4;
	typedef Vector<2, long> Long2;
	typedef Vector<3, long> Long3;
	typedef Vector<4, long> Long4;
	typedef Vector<2, unsigned long> ULong2;
	typedef Vector<3, unsigned long> ULong3;
	typedef Vector<4, unsigned long> ULong4;
	typedef Vector<2, float> Float2;
	typedef Vector<3, float> Float3;
	typedef Vector<4, float> Float4;
	typedef Vector<2, double> Double2;
	typedef Vector<3, double> Double3;
	typedef Vector<4, double> Double4;
	typedef Vector<2, long double> LDouble2;
	typedef Vector<3, long double> LDouble3;
	typedef Vector<4, long double> LDouble4;

	template<size_t D, typename N> constexpr auto Length(const Vector<D, N>& vec) -> decltype(std::sqrt(vec.X))
	{
		return std::sqrt(Dot(vec, vec));
	}

	template<size_t D, typename N> constexpr auto Normalize(const Vector<D, N>& vec) -> Vector<D, decltype(Length(vec))>
	{
		return 1 / Length(vec) * vec;
	}

	template<size_t D, typename N>
	constexpr auto Distance(const Vector<D, N>& v1, const Vector<D, N>& v2) -> decltype(Length(v1))
	{
		return Length(v1 - v2);
	}

	template<size_t D, typename N> constexpr auto Sqrt(const Vector<D, N>& vec) -> Vector<D, decltype(std::sqrt(vec.X))>
	{
		return Apply(vec, static_cast<decltype(std::sqrt(vec.X)) (*)(N)>(std::sqrt));
	}

	template<size_t D, typename N> constexpr auto InvSqrt(const Vector<D, N>& vec) -> decltype(Sqrt(vec))
	{
		return 1 / Sqrt(vec);
	}

	template<size_t D> constexpr Vector<D, float> InvSqrt(const Vector<D, float>& vec)
	{
		auto temp = vec;
		auto half = temp * 0.5f;
		auto p	  = reinterpret_cast<Vector<D, uint32_t>*>(&temp);
		auto i	  = 0x5F3759DFU - (*p >> 1);
		temp	  = *reinterpret_cast<Vector<D, float>*>(&i);
		return temp * (1.5f - half * temp * temp);
	}

	template<size_t D, typename N> constexpr auto Sin(const Vector<D, N>& vec) -> Vector<D, decltype(std::sin(vec.X))>
	{
		return Apply(vec, static_cast<decltype(std::sin(vec.X)) (*)(N)>(std::sin));
	}

	template<size_t D, typename N> constexpr auto Cos(const Vector<D, N>& vec) -> Vector<D, decltype(std::cos(vec.X))>
	{
		return Apply(vec, static_cast<decltype(std::cos(vec.X)) (*)(N)>(std::cos));
	}
}
