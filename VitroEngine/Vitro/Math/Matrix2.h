#pragma once

#include "_pch.h"
#include "Vitro/Math/Vector.h"

#define VTR_IS_SCALAR(O) typename = typename std::enable_if_t<std::is_arithmetic_v<O>, O>

namespace Vitro
{
	template<size_t R, size_t C, typename N> struct Matrix;

	template<size_t R, typename N> struct Matrix<R, 2, N> final
	{
	private:
		typedef Vector<R, N> Col;
		Col Raw[2];

	public:
		static constexpr std::enable_if_t<R == 2, Matrix<2, 2, N>> Identity()
		{
			return {1, 0, 0, 1};
		}

		template<typename O0, typename O1, typename O2, typename O3, typename std::enable_if_t<R == 2, O0>* = nullptr>
		constexpr Matrix(O0 x0, O1 y0, O2 x1, O3 y1) : Raw {Col(x0, y0), Col(x1, y1)}
		{}

		template<typename O0, typename O1, typename O2, typename O3, typename O4, typename O5,
				 typename std::enable_if_t<R == 3, O0>* = nullptr>
		constexpr Matrix(O0 x0, O1 y0, O2 z0, O3 x1, O4 y1, O5 z1) : Raw {Col(x0, y0, z0), Col(x1, y1, z1)}
		{}

		template<typename O0, typename O1, typename O2, typename O3, typename O4, typename O5, typename O6, typename O7,
				 typename std::enable_if_t<R == 4, O0>* = nullptr>
		constexpr Matrix(O0 x0, O1 y0, O2 z0, O3 w0, O4 x1, O5 y1, O6 z1, O7 w1) :
			Raw {Col(x0, y0, z0, w0), Col(x1, y1, z1, w1)}
		{}

		constexpr Matrix() : Matrix(0)
		{}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix(O scalar) : Raw {Col(scalar), Col(scalar)}
		{}

		template<typename O0, typename O1>
		constexpr Matrix(const Vector<R, O0>& v0, const Vector<R, O1>& v1) : Raw {v0, v1}
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

		template<typename O> constexpr Matrix<R, 2, N> operator+(const Matrix<R, 2, O>& m) const
		{
			return {Raw[0] + m[0], Raw[1] + m[1]};
		}

		template<typename O> constexpr Matrix<R, 2, N> operator-(const Matrix<R, 2, O>& m) const
		{
			return {Raw[0] - m[0], Raw[1] - m[1]};
		}

		template<typename O> constexpr Matrix<R, 2, N>& operator+=(const Matrix<R, 2, O>& m)
		{
			Raw[0] += m[0];
			Raw[1] += m[1];
			return *this;
		}

		template<typename O> constexpr Matrix<R, 2, N>& operator-=(const Matrix<R, 2, O>& m)
		{
			Raw[0] -= m[0];
			Raw[1] -= m[1];
			return *this;
		}

		template<typename O> constexpr bool operator==(const Matrix<R, 2, O>& m) const
		{
			return Raw[0] == m[0] && Raw[1] == m[1];
		}

		template<typename O> constexpr bool operator!=(const Matrix<R, 2, O>& m) const
		{
			return Raw[0] != m[0] || Raw[1] != m[1];
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 2, N> operator+(O scalar) const
		{
			return {Raw[0] + scalar, Raw[1] + scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 2, N> operator-(O scalar) const
		{
			return {Raw[0] - scalar, Raw[1] - scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 2, N> operator*(O scalar) const
		{
			return {Raw[0] * scalar, Raw[1] * scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 2, N> operator/(O scalar) const
		{
			return {Raw[0] / scalar, Raw[1] / scalar};
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 2, N>& operator+=(O scalar)
		{
			Raw[0] += scalar;
			Raw[1] += scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 2, N>& operator-=(O scalar)
		{
			Raw[0] -= scalar;
			Raw[1] -= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 2, N>& operator*=(O scalar)
		{
			Raw[0] *= scalar;
			Raw[1] *= scalar;
			return *this;
		}

		template<typename O, VTR_IS_SCALAR(O)> constexpr Matrix<R, 2, N>& operator/=(O scalar)
		{
			Raw[0] /= scalar;
			Raw[1] /= scalar;
			return *this;
		}

		constexpr Matrix<R, 2, N> operator+() const
		{
			return {+Raw[0], +Raw[1]};
		}

		constexpr Matrix<R, 2, N> operator-() const
		{
			return {-Raw[0], -Raw[1]};
		}

		constexpr Matrix<R, 2, N>& operator++()
		{
			++Raw[0];
			++Raw[1];
			return *this;
		}

		constexpr Matrix<R, 2, N>& operator--()
		{
			--Raw[0];
			--Raw[1];
			return *this;
		}

		constexpr Matrix<R, 2, N> operator++(int)
		{
			auto oldValue = *this;
			++Raw[0];
			++Raw[1];
			return oldValue;
		}

		constexpr Matrix<R, 2, N> operator--(int)
		{
			auto oldValue = *this;
			--Raw[0];
			--Raw[1];
			return oldValue;
		}

		std::string ToString() const
		{
			std::stringstream s;
			s << '[' << Raw[0].ToString() << ", ";
			s << Raw[1].ToString() << ']';
			return s.str();
		}
	};

	template<typename N, typename O>
	constexpr Matrix<2, 2, N> operator*(const Matrix<2, 2, N>& lm, const Matrix<2, 2, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y, lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y, lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y};
	}

	template<typename N, typename O>
	constexpr Matrix<2, 3, N> operator*(const Matrix<2, 2, N>& lm, const Matrix<2, 3, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y, lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y, lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y, lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y};
	}

	template<typename N, typename O>
	constexpr Matrix<2, 4, N> operator*(const Matrix<2, 2, N>& lm, const Matrix<2, 4, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y, lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y, lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y, lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y,
				lm[0].X * rm[3].X + lm[1].X * rm[3].Y, lm[0].Y * rm[3].X + lm[1].Y * rm[3].Y};
	}

	template<typename N, typename O>
	constexpr Matrix<3, 2, N> operator*(const Matrix<3, 2, N>& lm, const Matrix<2, 2, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y, lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y, lm[0].X * rm[1].X + lm[1].X * rm[1].Y,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y, lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y};
	}

	template<typename N, typename O>
	constexpr Matrix<3, 3, N> operator*(const Matrix<3, 2, N>& lm, const Matrix<2, 3, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y, lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y, lm[0].X * rm[1].X + lm[1].X * rm[1].Y,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y, lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y, lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y};
	}

	template<typename N, typename O>
	constexpr Matrix<3, 4, N> operator*(const Matrix<3, 2, N>& lm, const Matrix<2, 4, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y, lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y, lm[0].X * rm[1].X + lm[1].X * rm[1].Y,
				lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y, lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y,
				lm[0].X * rm[2].X + lm[1].X * rm[2].Y, lm[0].Y * rm[2].X + lm[1].Y * rm[2].Y,
				lm[0].Z * rm[2].X + lm[1].Z * rm[2].Y, lm[0].X * rm[3].X + lm[1].X * rm[3].Y,
				lm[0].Y * rm[3].X + lm[1].Y * rm[3].Y, lm[0].Z * rm[3].X + lm[1].Z * rm[3].Y};
	}

	template<typename N, typename O>
	constexpr Matrix<4, 2, N> operator*(const Matrix<4, 2, N>& lm, const Matrix<2, 2, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y, lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y, lm[0].W * rm[0].X + lm[1].W * rm[0].Y,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y, lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y, lm[0].W * rm[1].X + lm[1].W * rm[1].Y};
	}

	template<typename N, typename O>
	constexpr Matrix<4, 3, N> operator*(const Matrix<4, 2, N>& lm, const Matrix<2, 3, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y, lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y, lm[0].W * rm[0].X + lm[1].W * rm[0].Y,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y, lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y, lm[0].W * rm[1].X + lm[1].W * rm[1].Y,
				lm[0].X * rm[2].X + lm[2].X * rm[2].Y, lm[0].Y * rm[2].X + lm[2].Y * rm[2].Y,
				lm[0].Z * rm[2].X + lm[2].Z * rm[2].Y, lm[0].W * rm[2].X + lm[2].W * rm[2].Y};
	}

	template<typename N, typename O>
	constexpr Matrix<4, 4, N> operator*(const Matrix<4, 2, N>& lm, const Matrix<2, 4, O>& rm)
	{
		return {lm[0].X * rm[0].X + lm[1].X * rm[0].Y, lm[0].Y * rm[0].X + lm[1].Y * rm[0].Y,
				lm[0].Z * rm[0].X + lm[1].Z * rm[0].Y, lm[0].W * rm[0].X + lm[1].W * rm[0].Y,
				lm[0].X * rm[1].X + lm[1].X * rm[1].Y, lm[0].Y * rm[1].X + lm[1].Y * rm[1].Y,
				lm[0].Z * rm[1].X + lm[1].Z * rm[1].Y, lm[0].W * rm[1].X + lm[1].W * rm[1].Y,
				lm[0].X * rm[2].X + lm[2].X * rm[2].Y, lm[0].Y * rm[2].X + lm[2].Y * rm[2].Y,
				lm[0].Z * rm[2].X + lm[2].Z * rm[2].Y, lm[0].W * rm[2].X + lm[2].W * rm[2].Y,
				lm[0].X * rm[3].X + lm[2].X * rm[3].Y, lm[0].Y * rm[3].X + lm[2].Y * rm[3].Y,
				lm[0].Z * rm[3].X + lm[2].Z * rm[3].Y, lm[0].W * rm[3].X + lm[2].W * rm[3].Y};
	}

	template<typename N, typename O> constexpr Vector<2, N> operator*(const Matrix<2, 2, N>& m, const Vector<2, O>& v)
	{
		return {m[0].X * v.X + m[1].X * v.Y, m[0].Y * v.X + m[1].Y * v.Y};
	}

	template<typename N, typename O> constexpr Vector<2, N> operator*(const Vector<2, O>& v, const Matrix<2, 2, N>& m)
	{
		return {v.X * m[0].X + v.Y * m[0].Y, v.X * m[1].X + v.Y * m[1].Y};
	}

	template<typename N, typename O> constexpr Vector<3, N> operator*(const Matrix<3, 2, N>& m, const Vector<2, O>& v)
	{
		return {m[0].X * v.X + m[1].X * v.Y, m[0].Y * v.X + m[1].Y * v.Y, m[0].Z * v.X + m[1].Z * v.Y};
	}

	template<typename N, typename O> constexpr Vector<2, N> operator*(const Vector<3, O>& v, const Matrix<3, 2, N>& m)
	{
		return {v.X * m[0].X + v.Y * m[0].Y + v.Z * m[0].Z, v.X * m[1].X + v.Y * m[1].Y + v.Z * m[1].Z};
	}

	template<typename N, typename O> constexpr Vector<4, N> operator*(const Matrix<4, 2, N>& m, const Vector<2, O>& v)
	{
		return {m[0].X * v.X + m[1].X * v.Y, m[0].Y * v.X + m[1].Y * v.Y, m[0].Z * v.X + m[1].Z * v.Y,
				m[0].W * v.X + m[1].W * v.Y};
	}

	template<typename N, typename O> constexpr Vector<2, N> operator*(const Vector<4, O>& v, const Matrix<4, 2, N>& m)
	{
		return {v.X * m[0].X + v.Y * m[0].Y + v.Z * m[0].Z + v.W * m[0].W,
				v.X * m[1].X + v.Y * m[1].Y + v.Z * m[1].Z + v.W * m[1].W};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 2, N> operator+(O scalar, const Matrix<R, 2, N>& m)
	{
		return {scalar + m[0], scalar + m[1]};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 2, N> operator-(O scalar, const Matrix<R, 2, N>& m)
	{
		return {scalar - m[0], scalar - m[1]};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 2, N> operator*(O scalar, const Matrix<R, 2, N>& m)
	{
		return {scalar * m[0], scalar * m[1]};
	}

	template<size_t R, typename N, typename O, VTR_IS_SCALAR(O)>
	constexpr Matrix<R, 2, N> operator/(O scalar, const Matrix<R, 2, N>& m)
	{
		return {scalar / m[0], scalar / m[1]};
	}

	template<typename N> constexpr N Determinant(const Matrix<2, 2, N>& m)
	{
		return m[0].X * m[1].Y - m[1].X * m[0].Y;
	}

	template<typename N, typename F = float> constexpr Matrix<2, 2, F> Inverse(const Matrix<2, 2, N>& m)
	{
		F invDeterminant = static_cast<F>(1) / Determinant(m);

		return {+m[1].Y * invDeterminant, -m[0].Y * invDeterminant, -m[1].X * invDeterminant, +m[0].X * invDeterminant};
	}

	template<typename N> constexpr Matrix<2, 2, N> Transpose(const Matrix<2, 2, N>& m)
	{
		return {m[0].X, m[1].X, m[0].Y, m[1].Y};
	}

	template<typename N> constexpr Matrix<2, 3, N> Transpose(const Matrix<3, 2, N>& m)
	{
		return {m[0].X, m[1].X, m[0].Y, m[1].Y, m[0].Z, m[1].Z};
	}

	template<typename N> constexpr Matrix<2, 4, N> Transpose(const Matrix<4, 2, N>& m)
	{
		return {m[0].X, m[1].X, m[0].Y, m[1].Y, m[0].Z, m[1].Z, m[0].W, m[1].W};
	}
}
