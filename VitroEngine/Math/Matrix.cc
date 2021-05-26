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

	friend constexpr T determinant(Matrix mat) requires(R == C)
	{
		T det = 1;
		for(int i = 0; i < R; ++i)
		{
			int k = i;
			for(int j = i + 1; j < R; ++j)
				if(std::abs(mat[j][i]) > std::abs(mat[k][i]))
					k = j;

			if(std::abs(mat[k][i]) < std::numeric_limits<T>::epsilon())
			{
				det = 0;
				break;
			}

			std::swap(mat[i], mat[k]);
			if(i != k)
				det = -det;

			det *= mat[i][i];
			for(int j = i + 1; j < R; ++j)
				mat[i][j] /= mat[i][i];

			for(int j = 0; j < R; ++j)
				if(j != i && std::abs(mat[j][i]) > std::numeric_limits<T>::epsilon())
					for(int l = i + 1; l < R; ++l)
						mat[j][l] -= mat[i][l] * mat[j][i];
		}
		return det;
	}

	friend constexpr std::optional<Matrix> invert(const Matrix& mat) requires(R == C)
	{}
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
