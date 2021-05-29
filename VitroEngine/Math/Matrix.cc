module;
#include <cmath>
#include <format>
#include <optional>
export module Vitro.Math.Matrix;

import Vitro.Math.Vector;

export template<typename T, int R, int C> struct Matrix
{
	using Row = Vector<T, C>;
	Row rows[R];

	static constexpr Matrix identity() requires(R == C)
	{
		Matrix result;
		for(int r {}; r < R; ++r)
			result[r][r] = 1;
		return result;
	}

	template<Scalar S, int R2, int C2> constexpr operator Matrix<S, R2, C2>() const requires LosslesslyConvertibleTo<T, S>
	{
		return to<S, R2, C2>();
	}

	template<Scalar S, int R2, int C2>
	explicit constexpr operator Matrix<S, R2, C2>() const requires(!LosslesslyConvertibleTo<T, S>)
	{
		return to<S, R2, C2>();
	}

	constexpr Row& operator[](size_t index)
	{
		return rows[index];
	}

	constexpr const Row& operator[](size_t index) const
	{
		return rows[index];
	}

	template<Scalar S> constexpr bool operator==(const Matrix<S, R, C>& that) const
	{
		return std::equal(std::begin(rows), std::end(rows), std::begin(that.rows));
	}

	template<Scalar S> constexpr bool operator!=(const Matrix<S, R, C>& that) const
	{
		return !operator==(that);
	}

	constexpr auto operator+() const
	{
		Matrix<decltype(+rows[0][0]), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = +rows[r];
		return result;
	}

	template<Scalar S> constexpr auto operator+(const Matrix<S, R, C>& that) const
	{
		Matrix<decltype(rows[0][0] + that[0][0]), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = rows[r] + that[r];
		return result;
	}

	template<Scalar S> constexpr auto operator+(S scalar) const
	{
		Matrix<decltype(rows[0][0] + scalar), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = rows[r] + scalar;
		return result;
	}

	template<Scalar S> friend constexpr auto operator+(S scalar, const Matrix& mat)
	{
		return mat + scalar;
	}

	template<Scalar S> constexpr Matrix& operator+=(const Matrix<S, R, C>& that)
	{
		for(int r {}; r < R; ++r)
			rows[r] += that[r];
		return *this;
	}

	template<Scalar S> constexpr Matrix& operator+=(S scalar)
	{
		for(Row& row : rows)
			row += scalar;
		return *this;
	}

	constexpr auto operator-() const
	{
		Matrix<decltype(-rows[0][0]), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = -rows[r];
		return result;
	}

	template<Scalar S> constexpr auto operator-(const Matrix<S, R, C>& that) const
	{
		Matrix<decltype(rows[0][0] - that[0][0]), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = rows[r] - that[r];
		return result;
	}

	template<Scalar S> constexpr auto operator-(S scalar) const
	{
		Matrix<decltype(rows[0][0] - scalar), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = rows[r] - scalar;
		return result;
	}

	template<Scalar S> friend constexpr auto operator-(S scalar, const Matrix& mat)
	{
		Matrix<decltype(scalar - mat[0][0]), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = scalar - mat[r];
		return result;
	}

	template<Scalar S> constexpr Matrix& operator-=(const Matrix<S, R, C>& that)
	{
		for(int r {}; r < R; ++r)
			rows[r] -= that[r];
		return *this;
	}

	template<Scalar S> constexpr Matrix& operator-=(S scalar)
	{
		for(Row& row : rows)
			row -= scalar;
		return *this;
	}

	template<Scalar S, int C2> constexpr auto operator*(const Matrix<S, C, C2>& that) const
	{
		Matrix<decltype(rows[0][0] * that[0][0]), R, C2> result;
		for(int r {}; r < R; ++r)
			for(int c {}; c < C2; ++c)
				for(int i {}; i < C; ++i)
					result[r][c] += rows[r][i] * that[i][c];
		return result;
	}

	template<Scalar S> constexpr auto operator*(const Vector<S, C>& vec) const
	{
		Vector<decltype(rows[0][0] * vec[0]), R> result;
		for(int r {}; r < R; ++r)
			for(int c {}; c < C; ++c)
				result[r] += rows[c][r] * vec[c];
		return result;
	}

	template<Scalar S> friend constexpr auto operator*(const Vector<S, R>& vec, const Matrix& mat)
	{
		Vector<decltype(vec[0] * rows[0][0]), C> result;
		for(int c {}; c < C; ++c)
			for(int r {}; r < R; ++r)
				result[c] += vec[r] * mat.rows[r][c];
		return result;
	}

	template<Scalar S> constexpr auto operator*(S scalar) const
	{
		Matrix<decltype(rows[0][0] * scalar), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = rows[r] * scalar;
		return result;
	}

	template<Scalar S> friend constexpr auto operator*(S scalar, const Matrix& mat)
	{
		return mat * scalar;
	}

	template<Scalar S> constexpr Matrix& operator*=(const Matrix<S, R, C>& that)
	{
		return *this = *this * that;
	}

	template<Scalar S> constexpr Matrix& operator*=(S scalar)
	{
		for(Row& row : rows)
			row *= scalar;
		return *this;
	}

	template<Scalar S> constexpr auto operator/(S scalar) const
	{
		Matrix<decltype(rows[0][0] / scalar), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = rows[r] / scalar;
		return result;
	}

	template<Scalar S> friend constexpr auto operator/(S scalar, const Matrix& mat)
	{
		Matrix<decltype(scalar / mat[0][0]), R, C> result;
		for(int r {}; r < R; ++r)
			result[r] = scalar / mat[r];
		return result;
	}

	template<Scalar S> constexpr Matrix& operator/=(S scalar)
	{
		for(Row& row : rows)
			row /= scalar;
		return *this;
	}

	constexpr size_t rowSize() const
	{
		return R;
	}

	constexpr size_t columnSize() const
	{
		return C;
	}

	constexpr size_t size() const
	{
		return R * C;
	}

	constexpr T* data()
	{
		return rows[0].data;
	}

	constexpr const T* data() const
	{
		return rows[0].data;
	}

	template<Scalar S, int R2, int C2> constexpr Matrix<S, R2, C2> to() const
	{
		Matrix<S, R2, C2> result;
		for(int r {}; r < std::min(R, R2); ++r)
			result[r] = rows[r].to<S, C2>();
		return result;
	}

	std::string toString() const
	{
		auto str = std::format("[{}", rows[0].toString());

		for(int r = 1; r < R; ++r)
			str += std::format(", {}", rows[r].toString());

		return str + "]";
	}

	friend constexpr Matrix<T, C, R> transpose(const Matrix& mat)
	{
		Matrix<T, C, R> result;
		for(int c {}; c < C; ++c)
			for(int r {}; r < R; ++r)
				result[c][r] = mat[r][c];
		return result;
	}

	friend constexpr T determinant(const Matrix& mat) requires(R == C && R == 1)
	{
		return mat[0][0];
	}

	friend constexpr T determinant(const Matrix& mat) requires(R == C && R == 2)
	{
		return mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];
	}

	friend constexpr T determinant(const Matrix& mat) requires(R == C && R > 2)
	{
		Matrix<T, R - 1, R - 1> m;
		T det {};
		for(int j1 {}; j1 != R; ++j1)
		{
			for(int i = 1; i != R; ++i)
			{
				int j2 {};
				for(int j {}; j != R; ++j)
				{
					if(j == j1)
						continue;
					m[i - 1][j2] = mat[i][j];
					++j2;
				}
			}

			T d = determinant(m);
			if(j1 & 1)
				d = -d;
			det += mat[0][j1] * d;
		}
		return det;
	}

	friend constexpr Matrix inverse(const Matrix& mat) requires(R == C)
	{
		Matrix cofactor;
		Matrix<T, R - 1, R - 1> m;
		for(int j {}; j != R; ++j)
		{
			for(int i {}; i != R; ++i)
			{
				int i1 {};
				for(int ii {}; ii != R; ++ii)
				{
					if(ii == i)
						continue;
					int j1 {};
					for(int jj {}; jj != R; ++jj)
					{
						if(jj == j)
							continue;
						m[i1][j1] = mat[ii][jj];
						++j1;
					}
					++i1;
				}
				T det = determinant(m);
				if((i + j) & 1)
					det = -det;
				cofactor[i][j] = det;
			}
		}
		return transpose(cofactor) * (1 / determinant(mat));
	}
};

export using Float2x2 = Matrix<float, 2, 2>;
export using Float2x3 = Matrix<float, 2, 3>;
export using Float2x4 = Matrix<float, 2, 4>;
export using Float3x2 = Matrix<float, 3, 2>;
export using Float3x3 = Matrix<float, 3, 3>;
export using Float3x4 = Matrix<float, 3, 4>;
export using Float4x2 = Matrix<float, 4, 2>;
export using Float4x3 = Matrix<float, 4, 3>;
export using Float4x4 = Matrix<float, 4, 4>;
