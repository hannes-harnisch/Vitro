module;
#include <cmath>
#include <format>
export module vt.Core.Matrix;

import vt.Core.Vector;

namespace vt
{
	template<typename, int, int> struct Matrix;

	template<typename> struct IsScalar : std::true_type
	{};
	template<typename T, int D> struct IsScalar<Vector<T, D>> : std::false_type
	{};
	template<typename T, int R, int C> struct IsScalar<Matrix<T, R, C>> : std::false_type
	{};
	template<typename T>
	concept Scalar = IsScalar<T>::value;

	export template<typename T, int R, int C> struct Matrix
	{
		using Row = Vector<T, C>;
		Row rows[R];

		static constexpr Matrix identity() noexcept requires(R == C)
		{
			Matrix mat {};
			for(int r = 0; r != R; ++r)
				mat.rows[r][r] = 1;
			return mat;
		}

		static constexpr size_t row_size() noexcept
		{
			return R;
		}

		static constexpr size_t column_size() noexcept
		{
			return C;
		}

		static constexpr size_t size() noexcept
		{
			return R * C;
		}

		template<Scalar S, int R2, int C2>
		constexpr operator Matrix<S, R2, C2>() const noexcept requires LosslesslyConvertibleTo<T, S>
		{
			return to<S, R2, C2>();
		}

		template<Scalar S, int R2, int C2>
		explicit constexpr operator Matrix<S, R2, C2>() const noexcept requires(!LosslesslyConvertibleTo<T, S>)
		{
			return to<S, R2, C2>();
		}

		constexpr Row& operator[](size_t index) noexcept
		{
			return rows[index];
		}

		constexpr Row const& operator[](size_t index) const noexcept
		{
			return rows[index];
		}

		template<Scalar S> constexpr bool operator==(Matrix<S, R, C> const& that) const noexcept
		{
			return std::equal(std::begin(rows), std::end(rows), std::begin(that.rows));
		}

		template<Scalar S> constexpr bool operator!=(Matrix<S, R, C> const& that) const noexcept
		{
			return !operator==(that);
		}

		constexpr auto operator+() const noexcept
		{
			Matrix<decltype(+rows[0][0]), R, C> promoted;
			for(int r = 0; r != R; ++r)
				promoted.rows[r] = +rows[r];
			return promoted;
		}

		template<Scalar S> constexpr auto operator+(Matrix<S, R, C> const& that) const noexcept
		{
			Matrix<decltype(rows[0][0] + that[0][0]), R, C> sum;
			for(int r = 0; r != R; ++r)
				sum.rows[r] = rows[r] + that.rows[r];
			return sum;
		}

		constexpr auto operator+(Scalar auto scalar) const noexcept
		{
			Matrix<decltype(rows[0][0] + scalar), R, C> sum;
			for(int r = 0; r != R; ++r)
				sum.rows[r] = rows[r] + scalar;
			return sum;
		}

		friend constexpr auto operator+(Scalar auto scalar, Matrix const& mat) noexcept
		{
			return mat + scalar;
		}

		template<Scalar S> constexpr Matrix& operator+=(Matrix<S, R, C> const& that) noexcept
		{
			for(int r = 0; r != R; ++r)
				rows[r] += that.rows[r];
			return *this;
		}

		constexpr Matrix& operator+=(Scalar auto scalar) noexcept
		{
			for(Row& row : rows)
				row += scalar;
			return *this;
		}

		constexpr auto operator-() const noexcept
		{
			Matrix<decltype(-rows[0][0]), R, C> negated;
			for(int r = 0; r != R; ++r)
				negated.rows[r] = -rows[r];
			return negated;
		}

		template<Scalar S> constexpr auto operator-(Matrix<S, R, C> const& that) const noexcept
		{
			Matrix<decltype(rows[0][0] - that[0][0]), R, C> difference;
			for(int r = 0; r != R; ++r)
				difference.rows[r] = rows[r] - that.rows[r];
			return difference;
		}

		constexpr auto operator-(Scalar auto scalar) const noexcept
		{
			Matrix<decltype(rows[0][0] - scalar), R, C> difference;
			for(int r = 0; r != R; ++r)
				difference.rows[r] = rows[r] - scalar;
			return difference;
		}

		friend constexpr auto operator-(Scalar auto scalar, Matrix const& mat) noexcept
		{
			Matrix<decltype(scalar - mat[0][0]), R, C> difference;
			for(int r = 0; r != R; ++r)
				difference.rows[r] = scalar - mat.rows[r];
			return difference;
		}

		template<Scalar S> constexpr Matrix& operator-=(Matrix<S, R, C> const& that) noexcept
		{
			for(int r = 0; r != R; ++r)
				rows[r] -= that.rows[r];
			return *this;
		}

		constexpr Matrix& operator-=(Scalar auto scalar) noexcept
		{
			for(Row& row : rows)
				row -= scalar;
			return *this;
		}

		template<Scalar S, int C2> constexpr auto operator*(Matrix<S, C, C2> const& that) const noexcept
		{
			Matrix<decltype(rows[0][0] * that[0][0]), R, C2> product {};
			for(int r = 0; r != R; ++r)
				for(int c = 0; c != C2; ++c)
					for(int i = 0; i != C; ++i)
						product.rows[r][c] += rows[r][i] * that.rows[i][c];
			return product;
		}

		template<Scalar S> constexpr auto operator*(Vector<S, C> vec) const noexcept
		{
			Vector<decltype(rows[0][0] * vec[0]), R> product {};
			for(int r = 0; r != R; ++r)
				for(int c = 0; c != C; ++c)
					product[r] += rows[c][r] * vec[c];
			return product;
		}

		template<Scalar S> friend constexpr auto operator*(Vector<S, R> vec, Matrix const& mat) noexcept
		{
			Vector<decltype(vec[0] * rows[0][0]), C> product {};
			for(int c = 0; c != C; ++c)
				for(int r = 0; r != R; ++r)
					product[c] += vec[r] * mat.rows[r][c];
			return product;
		}

		constexpr auto operator*(Scalar auto scalar) const noexcept
		{
			Matrix<decltype(rows[0][0] * scalar), R, C> product;
			for(int r = 0; r != R; ++r)
				product.rows[r] = rows[r] * scalar;
			return product;
		}

		friend constexpr auto operator*(Scalar auto scalar, Matrix const& mat) noexcept
		{
			return mat * scalar;
		}

		template<Scalar S> constexpr Matrix& operator*=(Matrix<S, R, C> const& that) noexcept
		{
			return *this = *this * that;
		}

		constexpr Matrix& operator*=(Scalar auto scalar) noexcept
		{
			for(Row& row : rows)
				row *= scalar;
			return *this;
		}

		constexpr auto operator/(Scalar auto scalar) const noexcept
		{
			Matrix<decltype(rows[0][0] / scalar), R, C> quotient;
			for(int r = 0; r != R; ++r)
				quotient.rows[r] = rows[r] / scalar;
			return quotient;
		}

		friend constexpr auto operator/(Scalar auto scalar, Matrix const& mat) noexcept
		{
			Matrix<decltype(scalar / mat[0][0]), R, C> quotient;
			for(int r = 0; r != R; ++r)
				quotient.rows[r] = scalar / mat.rows[r];
			return quotient;
		}

		constexpr Matrix& operator/=(Scalar auto scalar) noexcept
		{
			for(Row& row : rows)
				row /= scalar;
			return *this;
		}

		template<Scalar S, int R2, int C2> constexpr Matrix<S, R2, C2> to() const noexcept
		{
			Matrix<S, R2, C2> cast;
			for(int r = 0; r != std::min(R, R2); ++r)
				cast.rows[r] = vector_cast<S, C2>(rows[r]);
			return cast;
		}

		std::string to_string() const
		{
			auto str = std::format("[{}", rows[0].to_string());

			for(int r = 1; r != R; ++r)
				str += std::format(", {}", rows[r].to_string());

			return str + ']';
		}

		friend constexpr Matrix<T, C, R> transpose(Matrix const& mat) noexcept
		{
			Matrix<T, C, R> transposed;
			for(int c = 0; c != C; ++c)
				for(int r = 0; r != R; ++r)
					transposed.rows[c][r] = mat.rows[r][c];
			return transposed;
		}

		friend constexpr T determinant(Matrix const& mat) noexcept requires(R == C && R == 1)
		{
			return mat.rows[0][0];
		}

		friend constexpr T determinant(Matrix const& mat) noexcept requires(R == C && R == 2)
		{
			return mat.rows[0][0] * mat.rows[1][1] - mat.rows[1][0] * mat.rows[0][1];
		}

		friend constexpr T determinant(Matrix const& mat) noexcept requires(R == C && R > 2)
		{
			Matrix<T, R - 1, R - 1> submatrix {};

			T det = 0;
			for(int i = 0; i != R; ++i)
			{
				for(int r = 1; r != R; ++r)
				{
					for(int c = 0, sub_c = 0; c != C; ++c)
					{
						if(c == i)
							continue;

						submatrix.rows[r - 1][sub_c] = mat.rows[r][c];
						++sub_c;
					}
				}
				T minor = determinant(submatrix);
				if(i & 1)
					minor = -minor;
				det += mat.rows[0][i] * minor;
			}
			return det;
		}

		friend constexpr auto inverse(Matrix const& mat) noexcept requires(R == C)
		{
			Matrix cofactor {};

			Matrix<T, R - 1, R - 1> submatrix {};
			for(int cof_c = 0; cof_c != C; ++cof_c)
			{
				for(int cof_r = 0; cof_r != R; ++cof_r)
				{
					for(int r = 0, sub_r = 0; r != R; ++r)
					{
						if(r == cof_r)
							continue;

						for(int c = 0, sub_c = 0; c != C; ++c)
						{
							if(c == cof_c)
								continue;

							submatrix.rows[sub_r][sub_c] = mat.rows[r][c];
							++sub_c;
						}
						++sub_r;
					}
					T minor = determinant(submatrix);
					if((cof_r + cof_c) & 1)
						minor = -minor;
					cofactor.rows[cof_r][cof_c] = minor;
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
