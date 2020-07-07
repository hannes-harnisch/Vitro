#pragma once

#include "_pch.h"
#include "Vitro/Math/Trigonometry.h"
#include "Vitro/Math/Vector.h"
#include "Vitro/Utility/Assert.h"

namespace Vitro
{
	template<typename N = float> struct Quaternion final
	{
		union
		{
			struct
			{
				N W, X, Y, Z;
			};
			struct
			{
				N A, B, C, D;
			};
			N Raw[4];
		};

		constexpr Quaternion() : Quaternion(0, 0, 0, 0)
		{}

		template<typename O0, typename O1, typename O2, typename O3>
		constexpr Quaternion(O0 w, O1 x, O2 y, O3 z) :
			Raw {static_cast<N>(w), static_cast<N>(x), static_cast<N>(y), static_cast<N>(z)}
		{}

		// Angle order: pitch, yaw, roll
		constexpr Quaternion(const Vector<3, N>& eulerAngles)
		{
			auto cos = Cos(eulerAngles * 0.5);
			auto sin = Sin(eulerAngles * 0.5);
			W		 = cos.X * cos.Y * cos.Z + sin.X * sin.Y * sin.Z;
			X		 = sin.X * cos.Y * cos.Z - cos.X * sin.Y * sin.Z;
			Y		 = cos.X * sin.Y * cos.Z + sin.X * cos.Y * sin.Z;
			Z		 = cos.X * cos.Y * sin.Z - sin.X * sin.Y * cos.Z;
		}

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

		template<typename O> constexpr Quaternion<N> operator+(const Quaternion<O>& q) const
		{
			return {W + q.W, X + q.X, Y + q.Y, Z + q.Z};
		}

		template<typename O> constexpr Quaternion<N> operator-(const Quaternion<O>& q) const
		{
			return {W - q.W, X - q.X, Y - q.Y, Z - q.Z};
		}

		template<typename O> constexpr Quaternion<N> operator*(const Quaternion<O>& q) const
		{
			return {W * q.W - X * q.X - Y * q.Y - Z * q.Z, W * q.X + X * q.W + Y * q.Z - Z * q.Y,
					W * q.Y + Y * q.W + Z * q.X - X * q.Z, W * q.Z + Z * q.W + X * q.Y - Y * q.X};
		}

		template<typename O> constexpr Quaternion<N>& operator+=(const Quaternion<O>& q)
		{
			W += q.W;
			X += q.X;
			Y += q.Y;
			Z += q.Z;
			return *this;
		}

		template<typename O> constexpr Quaternion<N>& operator-=(const Quaternion<O>& q)
		{
			W -= q.W;
			X -= q.X;
			Y -= q.Y;
			Z -= q.Z;
			return *this;
		}

		template<typename O> constexpr Quaternion<N>& operator*=(const Quaternion<O>& q)
		{
			return *this = *this * q;
		}

		template<typename O> constexpr bool operator==(const Quaternion<O>& q) const
		{
			return W == q.W && X == q.X && Y == q.Y && Z == q.Z;
		}

		template<typename O> constexpr bool operator!=(const Quaternion<O>& q) const
		{
			return W != q.W || X != q.X || Y != q.Y || Z != q.Z;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Quaternion<N> operator+(O scalar) const
		{
			return {W + scalar, X + scalar, Y + scalar, Z + scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Quaternion<N> operator-(O scalar) const
		{
			return {W - scalar, X - scalar, Y - scalar, Z - scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Quaternion<N> operator*(O scalar) const
		{
			return {W * scalar, X * scalar, Y * scalar, Z * scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Quaternion<N> operator/(O scalar) const
		{
			return {W / scalar, X / scalar, Y / scalar, Z / scalar};
		}

		template<typename O> constexpr Vector<3, N> operator*(const Vector<3, O>& v) const
		{
			Vector<3, N> qv(W, X, Y);
			Vector<3, N> uv(Cross(qv, v));
			Vector<3, N> uuv(Cross(qv, uv));
			return v + (uv * Z + uuv) * 2;
		}

		constexpr Vector<3, N> ToEulerAngles() const
		{
			Vector<3, N> angles;

			auto pitchSin = 2 * (W * Y - Z * X);
			if(std::abs(pitchSin) >= 1)
				angles.X = std::copysign(Pi<N> / 2, pitchSin);
			else
				angles.X = std::asin(pitchSin);

			auto yawSin = 2 * (W * Z + X * Y);
			auto yawCos = 1 - 2 * (Y * Y + Z * Z);
			angles.Y	= std::atan2(yawSin, yawCos);

			auto rollSin = 2 * (W * X + Y * Z);
			auto rollCos = 1 - 2 * (X * X + Y * Y);
			angles.Z	 = std::atan2(rollSin, rollCos);

			return angles;
		}

		std::string ToString() const
		{
			std::stringstream s;
			s << '(' << +W << ", " << +X << ", " << +Y << ", " << +Z << ')';
			return s.str();
		}
	};

	template<typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Quaternion<N> operator+(O scalar, const Quaternion<N>& q)
	{
		return {scalar + q.W, scalar + q.X, scalar + q.Y, scalar + q.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Quaternion<N> operator-(O scalar, const Quaternion<N>& q)
	{
		return {scalar - q.W, scalar - q.X, scalar - q.Y, scalar - q.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Quaternion<N> operator*(O scalar, const Quaternion<N>& q)
	{
		return {scalar * q.W, scalar * q.X, scalar * q.Y, scalar * q.Z};
	}

	template<typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Quaternion<N> operator/(O scalar, const Quaternion<N>& q)
	{
		return {scalar / q.W, scalar / q.X, scalar / q.Y, scalar / q.Z};
	}

	template<typename N, typename O> constexpr N Dot(const Quaternion<N>& lq, const Quaternion<O>& rq)
	{
		Quaternion<N> temp(lq.W * rq.W, lq.X * rq.X, lq.Y * rq.Y, lq.Z * rq.Z);
		return temp.W + temp.X + temp.Y + temp.Z;
	}

	template<typename N, typename O> constexpr Quaternion<N> Cross(const Quaternion<N>& lq, const Quaternion<O>& rq)
	{
		return {lq.W * rq.W - lq.X * rq.X - lq.Y * rq.Y - lq.Z * rq.Z,
				lq.W * rq.X + lq.X * rq.W + lq.Y * rq.Z - lq.Z * rq.Y,
				lq.W * rq.Y + lq.Y * rq.W + lq.Z * rq.X - lq.X * rq.Z,
				lq.W * rq.Z + lq.Z * rq.W + lq.X * rq.Y - lq.Y * rq.X};
	}

	template<typename N> constexpr auto Length(const Quaternion<N>& q) -> decltype(std::sqrt(q.W))
	{
		return std::sqrt(Dot(q, q));
	}

	template<typename N> constexpr auto Normalize(const Quaternion<N>& q) -> Quaternion<decltype(Length(q))>
	{
		return 1 / Length(q) * q;
	}
}
