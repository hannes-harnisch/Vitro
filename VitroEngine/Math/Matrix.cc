module;
#include <cmath>
#include <format>
export module Vitro.Math.Matrix;

import Vitro.Math.Vector;

namespace vt
{
	export template<typename T, int R, int C> struct Matrix
	{
		using Row = Vector<T, C>;
		Row rows[R];

		static constexpr Matrix identity() requires(R == C)
		{
			Matrix mat {};
			for(int r = 0; r < R; ++r)
				mat[r][r] = 1;
			return mat;
		}

		static constexpr size_t rowSize()
		{
			return R;
		}

		static constexpr size_t columnSize()
		{
			return C;
		}

		static constexpr size_t size()
		{
			return R * C;
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

		constexpr Row const& operator[](size_t index) const
		{
			return rows[index];
		}

		template<Scalar S> constexpr bool operator==(Matrix<S, R, C> const& that) const
		{
			return std::equal(std::begin(rows), std::end(rows), std::begin(that.rows));
		}

		template<Scalar S> constexpr bool operator!=(Matrix<S, R, C> const& that) const
		{
			return !operator==(that);
		}

		constexpr auto operator+() const
		{
			Matrix<decltype(+rows[0][0]), R, C> promoted;
			for(int r = 0; r < R; ++r)
				promoted[r] = +rows[r];
			return promoted;
		}

		template<Scalar S> constexpr auto operator+(Matrix<S, R, C> const& that) const
		{
			Matrix<decltype(rows[0][0] + that[0][0]), R, C> sum;
			for(int r = 0; r < R; ++r)
				sum[r] = rows[r] + that[r];
			return sum;
		}

		template<Scalar S> constexpr auto operator+(S scalar) const
		{
			Matrix<decltype(rows[0][0] + scalar), R, C> sum;
			for(int r = 0; r < R; ++r)
				sum[r] = rows[r] + scalar;
			return sum;
		}

		template<Scalar S> friend constexpr auto operator+(S scalar, Matrix const& mat)
		{
			return mat + scalar;
		}

		template<Scalar S> constexpr Matrix& operator+=(Matrix<S, R, C> const& that)
		{
			for(int r = 0; r < R; ++r)
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
			Matrix<decltype(-rows[0][0]), R, C> negated;
			for(int r = 0; r < R; ++r)
				negated[r] = -rows[r];
			return negated;
		}

		template<Scalar S> constexpr auto operator-(Matrix<S, R, C> const& that) const
		{
			Matrix<decltype(rows[0][0] - that[0][0]), R, C> difference;
			for(int r = 0; r < R; ++r)
				difference[r] = rows[r] - that[r];
			return difference;
		}

		template<Scalar S> constexpr auto operator-(S scalar) const
		{
			Matrix<decltype(rows[0][0] - scalar), R, C> difference;
			for(int r = 0; r < R; ++r)
				difference[r] = rows[r] - scalar;
			return difference;
		}

		template<Scalar S> friend constexpr auto operator-(S scalar, Matrix const& mat)
		{
			Matrix<decltype(scalar - mat[0][0]), R, C> difference;
			for(int r = 0; r < R; ++r)
				difference[r] = scalar - mat[r];
			return difference;
		}

		template<Scalar S> constexpr Matrix& operator-=(Matrix<S, R, C> const& that)
		{
			for(int r = 0; r < R; ++r)
				rows[r] -= that[r];
			return *this;
		}

		template<Scalar S> constexpr Matrix& operator-=(S scalar)
		{
			for(Row& row : rows)
				row -= scalar;
			return *this;
		}

		template<Scalar S, int C2> constexpr auto operator*(Matrix<S, C, C2> const& that) const
		{
			Matrix<decltype(rows[0][0] * that[0][0]), R, C2> product {};
			for(int r = 0; r < R; ++r)
				for(int c = 0; c < C2; ++c)
					for(int i = 0; i < C; ++i)
						product[r][c] += rows[r][i] * that[i][c];
			return product;
		}

		template<Scalar S> constexpr auto operator*(Vector<S, C> const& vec) const
		{
			Vector<decltype(rows[0][0] * vec[0]), R> product {};
			for(int r = 0; r < R; ++r)
				for(int c = 0; c < C; ++c)
					product[r] += rows[c][r] * vec[c];
			return product;
		}

		template<Scalar S> friend constexpr auto operator*(Vector<S, R> const& vec, Matrix const& mat)
		{
			Vector<decltype(vec[0] * rows[0][0]), C> product {};
			for(int c = 0; c < C; ++c)
				for(int r = 0; r < R; ++r)
					product[c] += vec[r] * mat.rows[r][c];
			return product;
		}

		template<Scalar S> constexpr auto operator*(S scalar) const
		{
			Matrix<decltype(rows[0][0] * scalar), R, C> product;
			for(int r = 0; r < R; ++r)
				product[r] = rows[r] * scalar;
			return product;
		}

		template<Scalar S> friend constexpr auto operator*(S scalar, Matrix const& mat)
		{
			return mat * scalar;
		}

		template<Scalar S> constexpr Matrix& operator*=(Matrix<S, R, C> const& that)
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
			Matrix<decltype(rows[0][0] / scalar), R, C> quotient;
			for(int r = 0; r < R; ++r)
				quotient[r] = rows[r] / scalar;
			return quotient;
		}

		template<Scalar S> friend constexpr auto operator/(S scalar, Matrix const& mat)
		{
			Matrix<decltype(scalar / mat[0][0]), R, C> quotient;
			for(int r = 0; r < R; ++r)
				quotient[r] = scalar / mat[r];
			return quotient;
		}

		template<Scalar S> constexpr Matrix& operator/=(S scalar)
		{
			for(Row& row : rows)
				row /= scalar;
			return *this;
		}

		constexpr T* data()
		{
			return rows[0].data;
		}

		constexpr T const* data() const
		{
			return rows[0].data;
		}

		template<Scalar S, int R2, int C2> constexpr Matrix<S, R2, C2> to() const
		{
			Matrix<S, R2, C2> cast;
			for(int r = 0; r < std::min(R, R2); ++r)
				cast[r] = rows[r].template to<S, C2>();
			return cast;
		}

		std::string toString() const
		{
			auto str = std::format("[{}", rows[0].toString());

			for(int r = 1; r < R; ++r)
				str += std::format(", {}", rows[r].toString());

			return str + ']';
		}

		friend constexpr Matrix<T, C, R> transpose(Matrix const& mat)
		{
			Matrix<T, C, R> transposed;
			for(int c = 0; c < C; ++c)
				for(int r = 0; r < R; ++r)
					transposed[c][r] = mat[r][c];
			return transposed;
		}

		friend constexpr T determinant(Matrix const& mat) requires(R == C && R == 1)
		{
			return mat[0][0];
		}

		friend constexpr T determinant(Matrix const& mat) requires(R == C && R == 2)
		{
			return mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1];
		}

		friend constexpr T determinant(Matrix const& mat) requires(R == C && R > 2)
		{
			Matrix<T, R - 1, R - 1> submatrix;
			T det {};
			for(int i = 0; i != R; ++i)
			{
				for(int r = 1; r != R; ++r)
				{
					for(int c = 0, subC {}; c != C; ++c)
					{
						if(c == i)
							continue;
						submatrix[r - 1][subC] = mat[r][c];
						++subC;
					}
				}
				T minor = determinant(submatrix);
				if(i & 1)
					minor = -minor;
				det += mat[0][i] * minor;
			}
			return det;
		}

		friend constexpr auto inverse(Matrix const& mat) requires(R == C)
		{
			Matrix cofactor;
			Matrix<T, R - 1, R - 1> submatrix;
			for(int cofC = 0; cofC != C; ++cofC)
			{
				for(int cofR = 0; cofR != R; ++cofR)
				{
					for(int r = 0, subR = 0; r != R; ++r)
					{
						if(r == cofR)
							continue;
						for(int c = 0, subC = 0; c != C; ++c)
						{
							if(c == cofC)
								continue;
							submatrix[subR][subC] = mat[r][c];
							++subC;
						}
						++subR;
					}
					T minor = determinant(submatrix);
					if((cofR + cofC) & 1)
						minor = -minor;
					cofactor[cofR][cofC] = minor;
				}
			}
			return transpose(cofactor) * (1.0f / determinant(mat));
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
}
