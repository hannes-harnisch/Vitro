#pragma once

#include "_pch.h"
#include "Vitro/Math/Vector.h"

#define VTR_IS_SCALAR(O) typename = typename std::enable_if_t<std::is_arithmetic_v<O>, O>

namespace Vitro
{
	template<size_t R, size_t C, typename N> struct Matrix;

	template<size_t R, typename N> struct Matrix<R, 3, N> final
	{
	private:
		typedef Vector<R, N> Col;
		Col Raw[3];

	public:
		static constexpr std::enable_if_t<R == 3, Matrix<3, 3, N>> Identity()
		{
			return {1, 0, 0, 0, 1, 0, 0, 0, 1};
		}

		template<typename O0, typename O1, typename O2, typename O3, typename O4, typename O5,
				 typename std::enable_if_t<R == 2, O0>* = nullptr>
		constexpr Matrix(O0 x0, O1 y0, O2 x1, O3 y1, O4 x2, O5 y2) : Raw {Col(x0, y0), Col(x1, y1), Col(x2, y2)}
		{}

		template<typename O0, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6, typename O7,
				 typename O8, typename std::enable_if_t<R == 3, O0>* = nullptr>
		constexpr Matrix(O0 x0, O1 y0, O2 z0, O3 x1, O4 y1, O5 z1, O6 x2, O7 y2, O8 z2) :
			Raw {Col(x0, y0, z0), Col(x1, y1, z1), Col(x2, y2, z2)}
		{}

		template<typename O0, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6, typename O7,
				 typename O8, typename O9, typename O10, typename O11, typename std::enable_if_t<R == 4, O0>* = nullptr>
		constexpr Matrix(O0 x0, O1 y0, O2 z0, O3 w0, O4 x1, O5 y1, O6 z1, O7 w1, O8 x2, O9 y2, O10 z2, O11 w2) :
			Raw {Col(x0, y0, z0, w0), Col(x1, y1, z1, w1), Col(x2, y2, z2, w2)}
		{}

		constexpr Matrix() : Matrix(0)
		{}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix(O scalar) : Raw {Col(scalar), Col(scalar), Col(scalar)}
		{}

		template<typename O0, typename O1, typename O2>
		constexpr Matrix(const Vector<R, O0>& v0, const Vector<R, O1>& v1, const Vector<R, O2>& v2) : Raw {v0, v1, v2}
		{}

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

		template<typename O> constexpr Matrix<R, 3, N> operator+(const Matrix<R, 3, O>& m) const
		{
			return {Raw[0] + m[0], Raw[1] + m[1], Raw[2] + m[2]};
		}

		template<typename O> constexpr Matrix<R, 3, N> operator-(const Matrix<R, 3, O>& m) const
		{
			return {Raw[0] - m[0], Raw[1] - m[1], Raw[2] - m[2]};
		}

		template<typename O> constexpr Matrix<R, 3, N>& operator+=(const Matrix<R, 3, O>& m)
		{
			Raw[0] += m[0];
			Raw[1] += m[1];
			Raw[2] += m[2];
			return *this;
		}

		template<typename O> constexpr Matrix<R, 3, N>& operator-=(const Matrix<R, 3, O>& m)
		{
			Raw[0] -= m[0];
			Raw[1] -= m[1];
			Raw[2] -= m[2];
			return *this;
		}

		template<typename O> constexpr bool operator==(const Matrix<R, 3, O>& m) const
		{
			return Raw[0] == m[0] && Raw[1] == m[1] && Raw[2] == m[2];
		}

		template<typename O> constexpr bool operator!=(const Matrix<R, 3, O>& m) const
		{
			return Raw[0] != m[0] || Raw[1] != m[1] || Raw[2] != m[2];
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 3, N> operator+(O scalar) const
		{
			return {Raw[0] + scalar, Raw[1] + scalar, Raw[2] + scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 3, N> operator-(O scalar) const
		{
			return {Raw[0] - scalar, Raw[1] - scalar, Raw[2] - scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 3, N> operator*(O scalar) const
		{
			return {Raw[0] * scalar, Raw[1] * scalar, Raw[2] * scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 3, N> operator/(O scalar) const
		{
			return {Raw[0] / scalar, Raw[1] / scalar, Raw[2] / scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 3, N>& operator+=(O scalar)
		{
			Raw[0] += scalar;
			Raw[1] += scalar;
			Raw[2] += scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 3, N>& operator-=(O scalar)
		{
			Raw[0] -= scalar;
			Raw[1] -= scalar;
			Raw[2] -= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 3, N>& operator*=(O scalar)
		{
			Raw[0] *= scalar;
			Raw[1] *= scalar;
			Raw[2] *= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 3, N>& operator/=(O scalar)
		{
			Raw[0] /= scalar;
			Raw[1] /= scalar;
			Raw[2] /= scalar;
			return *this;
		}

		constexpr Matrix<R, 3, N> operator+() const
		{
			return {+Raw[0], +Raw[1], +Raw[2]};
		}

		constexpr Matrix<R, 3, N> operator-() const
		{
			return {-Raw[0], -Raw[1], -Raw[2]};
		}

		constexpr Matrix<R, 3, N>& operator++()
		{
			++Raw[0];
			++Raw[1];
			++Raw[2];
			return *this;
		}

		constexpr Matrix<R, 3, N>& operator--()
		{
			--Raw[0];
			--Raw[1];
			--Raw[2];
			return *this;
		}

		constexpr Matrix<R, 3, N> operator++(int)
		{
			auto oldValue = *this;
			++Raw[0];
			++Raw[1];
			++Raw[2];
			return oldValue;
		}

		constexpr Matrix<R, 3, N> operator--(int)
		{
			auto oldValue = *this;
			--Raw[0];
			--Raw[1];
			--Raw[2];
			return oldValue;
		}

		std::string ToString() const
		{
			std::stringstream s;
			s << '[' << Raw[0].ToString() << ", ";
			s << Raw[1].ToString() << ", ";
			s << Raw[2].ToString() << ']';
			return s.str();
		}
	};

	template<typename N, typename O>
	constexpr Matrix<2, 2, N> operator*(const Matrix<2, 3, N>& lm, const Matrix<3, 2, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z};
	}

	template<typename N, typename O>
	constexpr Matrix<2, 3, N> operator*(const Matrix<2, 3, N>& lm, const Matrix<3, 3, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z};
	}

	template<typename N, typename O>
	constexpr Matrix<2, 4, N> operator*(const Matrix<2, 3, N>& lm, const Matrix<3, 4, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z,
				lm[0].X * rm[3].X + lm[1].X * rm[3].Y + lm[2].X * rm[3].Z,
				lm[0].Y * rm[3].X + lm[1].Y * rm[3].Y + lm[2].Y * rm[3].Z};
	}

	template<typename N, typename O>
	constexpr Matrix<3, 2, N> operator*(const Matrix<3, 3, N>& lm, const Matrix<3, 2, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z};
	}

	template<typename N, typename O>
	constexpr Matrix<3, 3, N> operator*(const Matrix<3, 3, N>& lm, const Matrix<3, 3, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y + lm[2].Z * rm[2].Z};
	}

	template<typename N, typename O>
	constexpr Matrix<3, 4, N> operator*(const Matrix<3, 3, N>& lm, const Matrix<3, 4, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y + lm[2].Z * rm[2].Z,
				lm[0].X * rm[3].X + lm[1].X * rm[3].Y + lm[2].X * rm[3].Z,
				lm[0].Y * rm[3].X + lm[1].Y * rm[3].Y + lm[2].Y * rm[3].Z,
				lm[0].Z * rm[3].X + lm[1].Z * rm[3].Y + lm[2].Z * rm[3].Z};
	}

	template<typename N, typename O>
	constexpr Matrix<4, 2, N> operator*(const Matrix<4, 3, N>& lm, const Matrix<3, 2, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z,
				lm[0].W * rm[0].X + lm[1].Z * rm[0].Y + lm[2].W * rm[0].Z,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z,
				lm[0].W * rm[1].X + lm[1].W * rm[1].Y + lm[2].W * rm[1].Z};
	}

	template<typename N, typename O>
	constexpr Matrix<4, 3, N> operator*(const Matrix<4, 3, N>& lm, const Matrix<3, 3, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z,
				lm[0].W * rm[0].X + lm[1].Z * rm[0].Y + lm[2].W * rm[0].Z,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z,
				lm[0].W * rm[1].X + lm[1].W * rm[1].Y + lm[2].W * rm[1].Z,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y + lm[2].Z * rm[2].Z,
				lm[0].W * rm[2].X + lm[1].W * rm[2].Y + lm[2].W * rm[2].Z};
	}

	template<typename N, typename O>
	constexpr Matrix<4, 4, N> operator*(const Matrix<4, 3, N>& lm, const Matrix<3, 4, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y + lm[2].X * rm[0].Z,
				lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y + lm[2].Y * rm[0].Z,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y + lm[2].Z * rm[0].Z,
				lm[0].W * rm[0].X + lm[1].Z * rm[0].Y + lm[2].W * rm[0].Z,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y + lm[2].X * rm[1].Z,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y + lm[2].Y * rm[1].Z,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y + lm[2].Z * rm[1].Z,
				lm[0].W * rm[1].X + lm[1].W * rm[1].Y + lm[2].W * rm[1].Z,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y + lm[2].X * rm[2].Z,
				lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y + lm[2].Y * rm[2].Z,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y + lm[2].Z * rm[2].Z,
				lm[0].W * rm[2].X + lm[1].W * rm[2].Y + lm[2].W * rm[2].Z,
				lm[0].X * rm[3].X + lm[1].X * rm[3].Y + lm[2].X * rm[3].Z,
				lm[0].Y * rm[3].X + lm[1].Y * rm[3].Y + lm[2].Y * rm[3].Z,
				lm[0].Z * rm[3].X + lm[1].Z * rm[3].Y + lm[2].Z * rm[3].Z,
				lm[0].W * rm[3].X + lm[1].W * rm[3].Y + lm[2].W * rm[3].Z};
	}

	template<typename N, typename O> constexpr Vector<2, N> operator*(const Matrix<2, 3, N>& m, const Vector<3, O>& v)
	{
		return {m[0].X * v.X + m[1].X * v.Y + m[2].X * v.Z, m[0].Y * v.X + m[1].Y * v.Y + m[2].Y * v.Z};
	}

	template<typename N, typename O> constexpr Vector<3, N> operator*(const Vector<2, O>& v, const Matrix<2, 3, N>& m)
	{
		return {v.X * m[0].X + v.Y * m[0].Y, v.X * m[1].X + v.Y * m[1].Y, v.X * m[2].X + v.Y * m[2].Y};
	}

	template<typename N, typename O> constexpr Vector<3, N> operator*(const Matrix<3, 3, N>& m, const Vector<3, O>& v)
	{
		return {m[0].X * v.X + m[1].X * v.Y + m[2].X * v.Z, m[0].Y * v.X + m[1].Y * v.Y + m[2].Y * v.Z,
				m[0].Z * v.X + m[1].Z * v.Y + m[2].Z * v.Z};
	}

	template<typename N, typename O> constexpr Vector<3, N> operator*(const Vector<3, O>& v, const Matrix<3, 3, N>& m)
	{
		return {v.X * m[0].X + v.Y * m[0].Y + v.Z * m[0].Z, v.X * m[1].X + v.Y * m[1].Y + v.Z * m[1].Z,
				v.X * m[2].X + v.Y * m[2].Y + v.Z * m[2].Z};
	}

	template<typename N, typename O> constexpr Vector<4, N> operator*(const Matrix<4, 3, N>& m, const Vector<3, O>& v)
	{
		return {m[0].X * v.X + m[1].X * v.Y + m[2].X * v.Z, m[0].Y * v.X + m[1].Y * v.Y + m[2].Y * v.Z,
				m[0].Z * v.X + m[1].Z * v.Y + m[2].Z * v.Z, m[0].W * v.X + m[1].W * v.Y + m[2].W * v.Z};
	}

	template<typename N, typename O> constexpr Vector<3, N> operator*(const Vector<4, O>& v, const Matrix<4, 3, N>& m)
	{
		return {v.X * m[0].X + v.Y * m[0].Y + v.Z * m[0].Z + v.W * m[0].W,
				v.X * m[1].X + v.Y * m[1].Y + v.Z * m[1].Z + v.W * m[1].W,
				v.X * m[2].X + v.Y * m[2].Y + v.Z * m[2].Z + v.W * m[2].W};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 3, N> operator+(O scalar, const Matrix<R, 3, N>& m)
	{
		return {scalar + m[0], scalar + m[1], scalar + m[2]};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 3, N> operator-(O scalar, const Matrix<R, 3, N>& m)
	{
		return {scalar - m[0], scalar - m[1], scalar - m[2]};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 3, N> operator*(O scalar, const Matrix<R, 3, N>& m)
	{
		return {scalar * m[0], scalar * m[1], scalar * m[2]};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 3, N> operator/(O scalar, const Matrix<R, 3, N>& m)
	{
		return {scalar / m[0], scalar / m[1], scalar / m[2]};
	}

	template<typename N> constexpr N Determinant(const Matrix<3, 3, N>& m)
	{
		return m[0].X * (m[1].Y * m[2].Z - m[2].Y * m[1].Z) - m[1].X * (m[0].Y * m[2].Z - m[2].Y * m[0].Z)
			   + m[2].X * (m[0].Y * m[1].Z - m[1].Y * m[0].Z);
	}

	template<typename N, typename F = float> constexpr Matrix<3, 3, F> Inverse(const Matrix<3, 3, N>& m)
	{
		F invDeterminant = static_cast<F>(1) / Determinant(m);

		return {+(m[1].Y * m[2].Z - m[2].Y * m[1].Z) * invDeterminant,
				-(m[0].Y * m[2].Z - m[2].Y * m[0].Z) * invDeterminant,
				+(m[0].Y * m[1].Z - m[1].Y * m[0].Z) * invDeterminant,
				-(m[1].X * m[2].Z - m[2].X * m[1].Z) * invDeterminant,
				+(m[0].X * m[2].Z - m[2].X * m[0].Z) * invDeterminant,
				-(m[0].X * m[1].Z - m[1].X * m[0].Z) * invDeterminant,
				+(m[1].X * m[2].Y - m[2].X * m[1].Y) * invDeterminant,
				-(m[0].X * m[2].Y - m[2].X * m[0].Y) * invDeterminant,
				+(m[0].X * m[1].Y - m[1].X * m[0].Y) * invDeterminant};
	}

	template<typename N> constexpr Matrix<3, 2, N> Transpose(const Matrix<2, 3, N>& m)
	{
		return {m[0].X, m[1].X, m[2].X, m[0].Y, m[1].Y, m[2].Y};
	}

	template<typename N> constexpr Matrix<3, 3, N> Transpose(const Matrix<3, 3, N>& m)
	{
		return {m[0].X, m[1].X, m[2].X, m[0].Y, m[1].Y, m[2].Y, m[0].Z, m[1].Z, m[2].Z};
	}

	template<typename N> constexpr Matrix<3, 4, N> Transpose(const Matrix<4, 3, N>& m)
	{
		return {m[0].X, m[1].X, m[2].X, m[0].Y, m[1].Y, m[2].Y, m[0].Z, m[1].Z, m[2].Z, m[0].W, m[1].W, m[2].W};
	}
}
