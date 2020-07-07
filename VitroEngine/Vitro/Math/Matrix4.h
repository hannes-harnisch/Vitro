#pragma once

#include "_pch.h"
#include "Vitro/Math/Quaternion.h"
#include "Vitro/Math/Vector.h"

#define VTR_IS_SCALAR(O) typename = typename std::enable_if_t<std::is_arithmetic_v<O>, O>
#define VTR_IS_FLOAT(N)	 typename = typename std::enable_if_t<std::is_floating_point_v<N>, N>

namespace Vitro
{
	template<size_t R, size_t C, typename N> struct Matrix;

	template<size_t R, typename N> struct Matrix<R, 4, N> final
	{
	private:
		typedef Vector<R, N> Col;
		Col Raw[4];

	public:
		static constexpr std::enable_if_t<R == 4, Matrix<4, 4, N>> Identity()
		{
			return {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		}

		template<typename O0, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6, typename O7,
				 typename std::enable_if_t<R == 2, O0>* = nullptr>
		constexpr Matrix(O0 x0, O1 y0, O2 x1, O3 y1, O4 x2, O5 y2, O6 x3, O7 y3) :
			Raw {Col(x0, y0), Col(x1, y1), Col(x2, y2), Col(x3, y3)}
		{}

		template<typename O0, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6, typename O7,
				 typename O8, typename O9, typename O10, typename O11, typename std::enable_if_t<R == 3, O0>* = nullptr>
		constexpr Matrix(O0 x0, O1 y0, O2 z0, O3 x1, O4 y1, O5 z1, O6 x2, O7 y2, O8 z2, O9 x3, O10 y3, O11 z3) :
			Raw {Col(x0, y0, z0), Col(x1, y1, z1), Col(x2, y2, z2), Col(x3, y3, z3)}
		{}

		template<typename O0, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6, typename O7,
				 typename O8, typename O9, typename O10, typename O11, typename O12, typename O13, typename O14,
				 typename O15, typename std::enable_if_t<R == 4, O0>* = nullptr>
		constexpr Matrix(O0 x0, O1 y0, O2 z0, O3 w0, O4 x1, O5 y1, O6 z1, O7 w1, O8 x2, O9 y2, O10 z2, O11 w2, O12 x3,
						 O13 y3, O14 z3, O15 w3) :
			Raw {Col(x0, y0, z0, w0), Col(x1, y1, z1, w1), Col(x2, y2, z2, w2), Col(x3, y3, z3, w3)}
		{}

		constexpr Matrix() : Matrix(0)
		{}

		template<typename O, VTR_IS_SCALAR(O)>
		constexpr Matrix(O scalar) : Raw {Col(scalar), Col(scalar), Col(scalar), Col(scalar)}
		{}

		template<typename O0, typename O1, typename O2, typename O3>
		constexpr Matrix(const Vector<R, O0>& v0, const Vector<R, O1>& v1, const Vector<R, O2>& v2,
						 const Vector<R, O3>& v3) :
			Raw {v0, v1, v2, v3}
		{}

		constexpr Matrix(std::enable_if_t<R == 4, const Quaternion<N>&> q)
		{
			N xx(q.X * q.X);
			N yy(q.Y * q.Y);
			N zz(q.Z * q.Z);
			N xz(q.X * q.Z);
			N xy(q.X * q.Y);
			N yz(q.Y * q.Z);
			N wx(q.W * q.X);
			N wy(q.W * q.Y);
			N wz(q.W * q.Z);
			Raw[0] = {1 - 2 * (yy * zz), 2 * (xy + wz), 2 * (xz - wy), 0};
			Raw[1] = {2 * (xy - wz), 1 - 2 * (xx + zz), 2 * (yz + wx), 0};
			Raw[2] = {2 * (xz + wy), 2 * (yz - wx), 1 - 2 * (xx + yy), 0};
			Raw[3] = {0, 0, 0, 1};
		}

		constexpr Col& operator[](size_t index)
		{
			AssertArrayRange(Raw, index);
			return Raw[index];
		}

		constexpr const Col& operator[](size_t index) const
		{
			AssertArrayRange(Raw, index);
			return Raw[index];
		}

		template<typename O> constexpr Matrix<R, 4, N> operator+(const Matrix<R, 4, O>& m) const
		{
			return {Raw[0] + m[0], Raw[1] + m[1], Raw[2] + m[2], Raw[3] + m[3]};
		}

		template<typename O> constexpr Matrix<R, 4, N> operator-(const Matrix<R, 4, O>& m) const
		{
			return {Raw[0] - m[0], Raw[1] - m[1], Raw[2] - m[2], Raw[3] - m[3]};
		}

		template<typename O> constexpr Matrix<R, 4, N>& operator+=(const Matrix<R, 4, O>& m)
		{
			Raw[0] += m[0];
			Raw[1] += m[1];
			Raw[2] += m[2];
			Raw[3] += m[3];
			return *this;
		}

		template<typename O> constexpr Matrix<R, 4, N>& operator-=(const Matrix<R, 4, O>& m)
		{
			Raw[0] -= m[0];
			Raw[1] -= m[1];
			Raw[2] -= m[2];
			Raw[3] -= m[3];
			return *this;
		}

		template<typename O> constexpr bool operator==(const Matrix<R, 4, O>& m) const
		{
			return Raw[0] == m[0] && Raw[1] == m[1] && Raw[2] == m[2] && Raw[3] == m[3];
		}

		template<typename O> constexpr bool operator!=(const Matrix<R, 4, O>& m) const
		{
			return Raw[0] != m[0] || Raw[1] != m[1] || Raw[2] != m[2] || Raw[3] != m[3];
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 4, N> operator+(O scalar) const
		{
			return {Raw[0] + scalar, Raw[1] + scalar, Raw[2] + scalar, Raw[3] + scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 4, N> operator-(O scalar) const
		{
			return {Raw[0] - scalar, Raw[1] - scalar, Raw[2] - scalar, Raw[3] - scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 4, N> operator*(O scalar) const
		{
			return {Raw[0] * scalar, Raw[1] * scalar, Raw[2] * scalar, Raw[3] * scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 4, N> operator/(O scalar) const
		{
			return {Raw[0] / scalar, Raw[1] / scalar, Raw[2] / scalar, Raw[3] / scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 4, N>& operator+=(O scalar)
		{
			Raw[0] += scalar;
			Raw[1] += scalar;
			Raw[2] += scalar;
			Raw[3] += scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 4, N>& operator-=(O scalar)
		{
			Raw[0] -= scalar;
			Raw[1] -= scalar;
			Raw[2] -= scalar;
			Raw[3] -= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 4, N>& operator*=(O scalar)
		{
			Raw[0] *= scalar;
			Raw[1] *= scalar;
			Raw[2] *= scalar;
			Raw[3] *= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 4, N>& operator/=(O scalar)
		{
			Raw[0] /= scalar;
			Raw[1] /= scalar;
			Raw[2] /= scalar;
			Raw[3] /= scalar;
			return *this;
		}

		constexpr Matrix<R, 4, N> operator+() const
		{
			return {+Raw[0], +Raw[1], +Raw[2], +Raw[3]};
		}

		constexpr Matrix<R, 4, N> operator-() const
		{
			return {-Raw[0], -Raw[1], -Raw[2], -Raw[3]};
		}

		constexpr Matrix<R, 4, N>& operator++()
		{
			++Raw[0];
			++Raw[1];
			++Raw[2];
			++Raw[3];
			return *this;
		}

		constexpr Matrix<R, 4, N>& operator--()
		{
			--Raw[0];
			--Raw[1];
			--Raw[2];
			--Raw[3];
			return *this;
		}

		constexpr Matrix<R, 4, N> operator++(int)
		{
			auto oldValue = *this;
			++Raw[0];
			++Raw[1];
			++Raw[2];
			++Raw[3];
			return oldValue;
		}

		constexpr Matrix<R, 4, N> operator--(int)
		{
			auto oldValue = *this;
			--Raw[0];
			--Raw[1];
			--Raw[2];
			--Raw[3];
			return oldValue;
		}

		std::string ToString() const
		{
			std::stringstream s;
			s << '[' << Raw[0].ToString() << ", ";
			s << Raw[1].ToString() << ", ";
			s << Raw[2].ToString() << ", ";
			s << Raw[3].ToString() << ']';
			return s.str();
		}
	};

	template<typename N, typename O>
	constexpr Matrix<2, 2, N> operator*(const Matrix<2, 4, N>& lm, const Matrix<4, 2, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z + lm[3].X * rm[0].W,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z + lm[3].Y * rm[0].W,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z + lm[3].X * rm[1].W,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z + lm[3].Y * rm[1].W};
	}

	template<typename N, typename O>
	constexpr Matrix<2, 3, N> operator*(const Matrix<2, 4, N>& lm, const Matrix<4, 3, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z + lm[3].X * rm[0].W,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z + lm[3].Y * rm[0].W,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z + lm[3].X * rm[1].W,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z + lm[3].Y * rm[1].W,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z + lm[3].X * rm[2].W,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z + lm[3].Y * rm[2].W};
	}

	template<typename N, typename O>
	constexpr Matrix<2, 4, N> operator*(const Matrix<2, 4, N>& lm, const Matrix<4, 4, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z + lm[3].X * rm[0].W,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z + lm[3].Y * rm[0].W,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z + lm[3].X * rm[1].W,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z + lm[3].Y * rm[1].W,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z + lm[3].X * rm[2].W,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z + lm[3].Y * rm[2].W,
				lm[0].X * rm[3].X + lm[1].X * rm[3].Y + lm[2].X * rm[3].Z + lm[3].X * rm[3].W,
				lm[0].Y * rm[3].X + lm[1].Y * rm[3].Y + lm[2].Y * rm[3].Z + lm[3].Y * rm[3].W};
	}

	template<typename N, typename O>
	constexpr Matrix<3, 2, N> operator*(const Matrix<3, 4, N>& lm, const Matrix<4, 2, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z + lm[3].X * rm[0].W,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z + lm[3].Y * rm[0].W,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z + lm[3].Z * rm[0].W,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z + lm[3].X * rm[1].W,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z + lm[3].Y * rm[1].W,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z + lm[3].Z * rm[1].W};
	}

	template<typename N, typename O>
	constexpr Matrix<3, 3, N> operator*(const Matrix<3, 4, N>& lm, const Matrix<4, 3, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z + lm[3].X * rm[0].W,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z + lm[3].Y * rm[0].W,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z + lm[3].Z * rm[0].W,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z + lm[3].X * rm[1].W,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z + lm[3].Y * rm[1].W,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z + lm[3].Z * rm[1].W,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z + lm[3].X * rm[2].W,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z + lm[3].Y * rm[2].W,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y + lm[2].Z * rm[2].Z + lm[3].Z * rm[2].W};
	}

	template<typename N, typename O>
	constexpr Matrix<3, 4, N> operator*(const Matrix<3, 4, N>& lm, const Matrix<4, 4, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z + lm[3].X * rm[0].W,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z + lm[3].Y * rm[0].W,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z + lm[3].Z * rm[0].W,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z + lm[3].X * rm[1].W,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z + lm[3].Y * rm[1].W,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z + lm[3].Z * rm[1].W,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z + lm[3].X * rm[2].W,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z + lm[3].Y * rm[2].W,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y + lm[2].Z * rm[2].Z + lm[3].Z * rm[2].W,
				lm[0].X * rm[3].X + lm[1].X * rm[3].Y + lm[2].X * rm[3].Z + lm[3].X * rm[3].W,
				lm[0].Y * rm[3].X + lm[1].Y * rm[3].Y + lm[2].Y * rm[3].Z + lm[3].Y * rm[3].W,
				lm[0].Z * rm[3].X + lm[1].Z * rm[3].Y + lm[2].Z * rm[3].Z + lm[3].Z * rm[3].W};
	}

	template<typename N, typename O>
	constexpr Matrix<4, 2, N> operator*(const Matrix<4, 4, N>& lm, const Matrix<4, 2, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z + lm[3].X * rm[0].W,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z + lm[3].Y * rm[0].W,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z + lm[3].Z * rm[0].W,
				lm[0].W * rm[0].X + lm[1].W * rm[0].Y + lm[2].W * rm[0].Z + lm[3].W * rm[0].W,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z + lm[3].X * rm[1].W,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z + lm[3].Y * rm[1].W,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z + lm[3].Z * rm[1].W,
				lm[0].W * rm[1].X + lm[1].W * rm[1].Y + lm[2].W * rm[1].Z + lm[3].W * rm[1].W};
	}

	template<typename N, typename O>
	constexpr Matrix<4, 3, N> operator*(const Matrix<4, 4, N>& lm, const Matrix<4, 3, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z + lm[3].X * rm[0].W,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z + lm[3].Y * rm[0].W,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z + lm[3].Z * rm[0].W,
				lm[0].W * rm[0].X + lm[1].W * rm[0].Y + lm[2].W * rm[0].Z + lm[3].W * rm[0].W,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z + lm[3].X * rm[1].W,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z + lm[3].Y * rm[1].W,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z + lm[3].Z * rm[1].W,
				lm[0].W * rm[1].X + lm[1].W * rm[1].Y + lm[2].W * rm[1].Z + lm[3].W * rm[1].W,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z + lm[3].X * rm[2].W,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z + lm[3].Y * rm[2].W,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y + lm[2].Z * rm[2].Z + lm[3].Z * rm[2].W,
				lm[0].W * rm[2].X + lm[1].W * rm[2].Y + lm[2].W * rm[2].Z + lm[3].W * rm[2].W};
	}

	template<typename N, typename O>
	constexpr Matrix<4, 4, N> operator*(const Matrix<4, 4, N>& lm, const Matrix<4, 4, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z + lm[3].X * rm[0].W,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z + lm[3].Y * rm[0].W,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z + lm[3].Z * rm[0].W,
				lm[0].W * rm[0].X + lm[1].W * rm[0].Y + lm[2].W * rm[0].Z + lm[3].W * rm[0].W,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z + lm[3].X * rm[1].W,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z + lm[3].Y * rm[1].W,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z + lm[3].Z * rm[1].W,
				lm[0].W * rm[1].X + lm[1].W * rm[1].Y + lm[2].W * rm[1].Z + lm[3].W * rm[1].W,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z + lm[3].X * rm[2].W,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z + lm[3].Y * rm[2].W,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y + lm[2].Z * rm[2].Z + lm[3].Z * rm[2].W,
				lm[0].W * rm[2].X + lm[1].W * rm[2].Y + lm[2].W * rm[2].Z + lm[3].W * rm[2].W,
				lm[0].X * rm[3].X + lm[1].X * rm[3].Y + lm[2].X * rm[3].Z + lm[3].X * rm[3].W,
				lm[0].Y * rm[3].X + lm[1].Y * rm[3].Y + lm[2].Y * rm[3].Z + lm[3].Y * rm[3].W,
				lm[0].Z * rm[3].X + lm[1].Z * rm[3].Y + lm[2].Z * rm[3].Z + lm[3].Z * rm[3].W,
				lm[0].W * rm[3].X + lm[1].W * rm[3].Y + lm[2].W * rm[3].Z + lm[3].W * rm[3].W};
	}

	template<typename N, typename O> constexpr Vector<2, N> operator*(const Matrix<2, 4, N>& m, const Vector<4, O>& v)
	{
		return {m[0].X * v.X + m[1].X * v.Y + m[2].X * v.Z + m[3].X * v.W,
				m[0].Y * v.X + m[1].Y * v.Y + m[2].Y * v.Z + m[3].Y * v.W};
	}

	template<typename N, typename O> constexpr Vector<4, N> operator*(const Vector<2, O>& v, const Matrix<2, 4, N>& m)
	{
		return {v.X * m[0].X + v.Y * m[0].Y, v.X * m[1].X + v.Y * m[1].Y, v.X * m[2].X + v.Y * m[2].Y,
				v.X * m[3].X + v.Y * m[3].Y};
	}

	template<typename N, typename O> constexpr Vector<3, N> operator*(const Matrix<3, 4, N>& m, const Vector<4, O>& v)
	{
		return {m[0].X * v.X + m[1].X * v.Y + m[2].X * v.Z + m[3].X * v.W,
				m[0].Y * v.X + m[1].Y * v.Y + m[2].Y * v.Z + m[3].Y * v.W,
				m[0].Z * v.X + m[1].Z * v.Y + m[2].Z * v.Z + m[3].Z * v.W};
	}

	template<typename N, typename O> constexpr Vector<4, N> operator*(const Vector<3, O>& v, const Matrix<3, 4, N>& m)
	{
		return {v.X * m[0].X + v.Y * m[0].Y + v.Z * m[0].Z, v.X * m[1].X + v.Y * m[1].Y + v.Z * m[1].Z,
				v.X * m[2].X + v.Y * m[2].Y + v.Z * m[2].Z, v.X * m[3].X + v.Y * m[3].Y + v.Z * m[3].Z};
	}

	template<typename N, typename O> constexpr Vector<4, N> operator*(const Matrix<4, 4, N>& m, const Vector<4, O>& v)
	{
		return {m[0].X * v.X + m[1].X * v.Y + m[2].X * v.Z + m[3].X * v.W,
				m[0].Y * v.X + m[1].Y * v.Y + m[2].Y * v.Z + m[3].Y * v.W,
				m[0].Z * v.X + m[1].Z * v.Y + m[2].Z * v.Z + m[3].Z * v.W,
				m[0].W * v.X + m[1].W * v.Y + m[2].W * v.Z + m[3].W * v.W};
	}

	template<typename N, typename O> constexpr Vector<4, N> operator*(const Vector<4, O>& v, const Matrix<4, 4, N>& m)
	{
		return {v.X * m[0].X + v.Y * m[0].Y + v.Z * m[0].Z + v.W * m[0].W,
				v.X * m[1].X + v.Y * m[1].Y + v.Z * m[1].Z + v.W * m[1].W,
				v.X * m[2].X + v.Y * m[2].Y + v.Z * m[2].Z + v.W * m[2].W,
				v.X * m[3].X + v.Y * m[3].Y + v.Z * m[3].Z + v.W * m[3].W};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 4, N> operator+(O scalar, const Matrix<R, 4, N>& m)
	{
		return {scalar + m[0], scalar + m[1], scalar + m[2], scalar + m[3]};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 4, N> operator-(O scalar, const Matrix<R, 4, N>& m)
	{
		return {scalar - m[0], scalar - m[1], scalar - m[2], scalar - m[3]};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 4, N> operator*(O scalar, const Matrix<R, 4, N>& m)
	{
		return {scalar * m[0], scalar * m[1], scalar * m[2], scalar * m[3]};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 4, N> operator/(O scalar, const Matrix<R, 4, N>& m)
	{
		return {scalar / m[0], scalar / m[1], scalar / m[2], scalar / m[3]};
	}

	template<typename N> constexpr N Determinant(const Matrix<4, 4, N>& m)
	{
		N subFactor0 = m[2].Z * m[3].W - m[3].Z * m[2].W;
		N subFactor1 = m[2].Y * m[3].W - m[3].Y * m[2].W;
		N subFactor2 = m[2].Y * m[3].Z - m[3].Y * m[2].Z;
		N subFactor3 = m[2].X * m[3].W - m[3].X * m[2].W;
		N subFactor4 = m[2].X * m[3].Z - m[3].X * m[2].Z;
		N subFactor5 = m[2].X * m[3].Y - m[3].X * m[2].Y;

		N x = +(m[1].Y * subFactor0 - m[1].Z * subFactor1 + m[1].W * subFactor2);
		N y = -(m[1].X * subFactor0 - m[1].Z * subFactor3 + m[1].W * subFactor4);
		N z = +(m[1].X * subFactor1 - m[1].Y * subFactor3 + m[1].W * subFactor5);
		N w = -(m[1].X * subFactor2 - m[1].Y * subFactor4 + m[1].Z * subFactor5);

		auto coeffs = Vector<4, N>(x, y, z, w) * m[0];
		return coeffs.X + coeffs.Y + coeffs.Z + coeffs.W;
	}

	template<typename N, typename F = float> constexpr Matrix<4, 4, F> Inverse(const Matrix<4, 4, N>& m)
	{
		N coef00 = m[2].Z * m[3].W - m[3].Z * m[2].W;
		N coef02 = m[1].Z * m[3].W - m[3].Z * m[1].W;
		N coef03 = m[1].Z * m[2].W - m[2].Z * m[1].W;

		N coef04 = m[2].Y * m[3].W - m[3].Y * m[2].W;
		N coef06 = m[1].Y * m[3].W - m[3].Y * m[1].W;
		N coef07 = m[1].Y * m[2].W - m[2].Y * m[1].W;

		N coef08 = m[2].Y * m[3].Z - m[3].Y * m[2].Z;
		N coef10 = m[1].Y * m[3].Z - m[3].Y * m[1].Z;
		N coef11 = m[1].Y * m[2].Z - m[2].Y * m[1].Z;

		N coef12 = m[2].X * m[3].W - m[3].X * m[2].W;
		N coef14 = m[1].X * m[3].W - m[3].X * m[1].W;
		N coef15 = m[1].X * m[2].W - m[2].X * m[1].W;

		N coef16 = m[2].X * m[3].Z - m[3].X * m[2].Z;
		N coef18 = m[1].X * m[3].Z - m[3].X * m[1].Z;
		N coef19 = m[1].X * m[2].Z - m[2].X * m[1].Z;

		N coef20 = m[2].X * m[3].Y - m[3].X * m[2].Y;
		N coef22 = m[1].X * m[3].Y - m[3].X * m[1].Y;
		N coef23 = m[1].X * m[2].Y - m[2].X * m[1].Y;

		Vector<4, N> fac0(coef00, coef00, coef02, coef03);
		Vector<4, N> fac1(coef04, coef04, coef06, coef07);
		Vector<4, N> fac2(coef08, coef08, coef10, coef11);
		Vector<4, N> fac3(coef12, coef12, coef14, coef15);
		Vector<4, N> fac4(coef16, coef16, coef18, coef19);
		Vector<4, N> fac5(coef20, coef20, coef22, coef23);

		Vector<4, N> vec0(m[1].X, m[0].X, m[0].X, m[0].X);
		Vector<4, N> vec1(m[1].Y, m[0].Y, m[0].Y, m[0].Y);
		Vector<4, N> vec2(m[1].Z, m[0].Z, m[0].Z, m[0].Z);
		Vector<4, N> vec3(m[1].W, m[0].W, m[0].W, m[0].W);

		Vector<4, N> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
		Vector<4, N> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
		Vector<4, N> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
		Vector<4, N> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

		Vector<4, N> signA(+1, -1, +1, -1);
		Vector<4, N> signB(-1, +1, -1, +1);
		Matrix<4, 4, F> inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);
		Vector<4, N> row(inverse[0].X, inverse[1].X, inverse[2].X, inverse[3].X);
		Vector<4, N> dot(m[0] * row);
		N dotProduct = dot.X + dot.Y + dot.Z + dot.W;
		return inverse * (static_cast<F>(1) / dotProduct);
	}

	template<typename N> constexpr Matrix<4, 2, N> Transpose(const Matrix<2, 4, N>& m)
	{
		return {m[0].X, m[1].X, m[2].X, m[3].X, m[0].Y, m[1].Y, m[2].Y, m[3].Y};
	}

	template<typename N> constexpr Matrix<4, 3, N> Transpose(const Matrix<3, 4, N>& m)
	{
		return {m[0].X, m[1].X, m[2].X, m[3].X, m[0].Y, m[1].Y, m[2].Y, m[3].Y, m[0].Z, m[1].Z, m[2].Z, m[3].Z};
	}

	template<typename N> constexpr Matrix<4, 4, N> Transpose(const Matrix<4, 4, N>& m)
	{
		return {m[0].X, m[1].X, m[2].X, m[3].X, m[0].Y, m[1].Y, m[2].Y, m[3].Y,
				m[0].Z, m[1].Z, m[2].Z, m[3].Z, m[0].W, m[1].W, m[2].W, m[3].W};
	}

	// Left-hand coordinates, Z normalization between 0 and 1
	template<typename N, VTR_IS_FLOAT(N)>
	constexpr Matrix<4, 4, N> Orthographic(N left, N right, N bottom, N top, N nearZ, N farZ)
	{
		auto ortho = Matrix<4, 4, N>::Identity();
		ortho[0].X = 2 / (right - left);
		ortho[1].Y = 2 / (top - bottom);
		ortho[2].Z = 2 / (farZ - nearZ);
		ortho[3].X = -(right + left) / (right - left);
		ortho[3].Y = -(top + bottom) / (top - bottom);
		ortho[3].Z = -nearZ / (farZ - nearZ);
		return ortho;
	}

	// Left-hand coordinates, Z normalization between 0 and 1
	template<typename N, VTR_IS_FLOAT(N), typename I>
	constexpr Matrix<4, 4, N> Perspective(N fovInRadians, I width, I height, N nearZ, N farZ)
	{
		N aspect = width / static_cast<N>(height);
		AssertDebug(std::abs(aspect - std::numeric_limits<N>::epsilon()) > 0, "Bad aspect ratio.");
		N fovTan = std::tan(fovInRadians / 2);

		Matrix<4, 4, N> perspective;
		perspective[0].X = 1 / (aspect * fovTan);
		perspective[1].Y = 1 / fovTan;
		perspective[2].Z = farZ / (farZ - nearZ);
		perspective[2].W = 1;
		perspective[3].Z = -(farZ * nearZ) / (farZ - nearZ);
		return perspective;
	}

	// Left-hand coordinates
	template<typename N, VTR_IS_FLOAT(N)>
	constexpr Matrix<4, 4, N> LookAt(const Vector<3, N>& eye, const Vector<3, N>& at, const Vector<3, N>& up)
	{
		Vector<3, N> zAxis = Normalize(at - eye);
		Vector<3, N> xAxis = Normalize(Cross(up, zAxis));
		Vector<3, N> yAxis = Cross(zAxis, xAxis);
		return {xAxis.X, yAxis.X, zAxis.X, 0, xAxis.Y,			yAxis.Y,		  zAxis.Y,			0,
				xAxis.Z, yAxis.Z, zAxis.Z, 0, -Dot(xAxis, eye), -Dot(yAxis, eye), -Dot(zAxis, eye), 1};
	}

	template<typename N, VTR_IS_FLOAT(N)>
	constexpr Matrix<4, 4, N> Translate(const Matrix<4, 4, N>& m, const Vector<3, N>& v)
	{
		auto result = m;
		result[3]	= m[0] * v.X + m[1] * v.Y + m[2] * v.Z + m[3];
		return result;
	}

	template<typename N, VTR_IS_FLOAT(N)> constexpr Matrix<4, 4, N> Translate(const Vector<3, N>& v)
	{
		auto result = Matrix<4, 4, N>::Identity();
		result[3]	= result[0] * v.X + result[1] * v.Y + result[2] * v.Z + result[3];
		return result;
	}

	template<typename N, VTR_IS_FLOAT(N)>
	constexpr Matrix<4, 4, N> Rotate(const Matrix<4, 4, N>& m, N angle, const Vector<3, N>& v)
	{
		N cos = std::cos(angle);
		N sin = std::sin(angle);

		Vector<3, N> axis(Normalize(v));
		Vector<3, N> temp((1 - cos) * axis);

		Matrix<4, 4, N> rotated;
		rotated[0].X = cos + temp.X * axis.X;
		rotated[0].Y = temp.X * axis.Y + sin * axis.Z;
		rotated[0].Z = temp.X * axis.Z - sin * axis.Y;

		rotated[1].X = temp.Y * axis.X - sin * axis.Z;
		rotated[1].Y = cos + temp.Y * axis.Y;
		rotated[1].Z = temp.Y * axis.Z + sin * axis.X;

		rotated[2].X = temp.Z * axis.X + sin * axis.Y;
		rotated[2].Y = temp.Z * axis.Y - sin * axis.X;
		rotated[2].Z = cos + temp.Z * axis.Z;

		Matrix<4, 4, N> result;
		result[0] = m[0] * rotated[0].X + m[1] * rotated[0].Y + m[2] * rotated[0].Z;
		result[1] = m[0] * rotated[1].X + m[1] * rotated[1].Y + m[2] * rotated[1].Z;
		result[2] = m[0] * rotated[2].X + m[1] * rotated[2].Y + m[2] * rotated[2].Z;
		result[3] = m[3];
		return result;
	}

	template<typename N, VTR_IS_FLOAT(N)>
	constexpr Matrix<4, 4, N> Scale(const Matrix<4, 4, N>& m, const Vector<3, N>& v)
	{
		Matrix<4, 4, N> result;
		result[0] = m[0] * v.X;
		result[1] = m[1] * v.Y;
		result[2] = m[2] * v.Z;
		result[3] = m[3];
		return result;
	}
}
