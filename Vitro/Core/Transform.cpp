module;
#include "Core/Macros.hpp"

#include <cmath>
#include <limits>
export module vt.Core.Transform;

import vt.Core.Matrix;
import vt.Core.Vector;

namespace vt
{
	export Float4x4 project_orthographic(float left, float right, float bottom, float top, float near_z, float far_z)
	{
		return {{
			{2 / (right - left), 0, 0, 0},
			{0, 2 / (top - bottom), 0, 0},
			{0, 0, 2 / (far_z - near_z), 0},
			{-(right + left) / (right - left), -(top + bottom) / (top - bottom), -near_z / (far_z - near_z), 1},
		}};
	}

	export Float4x4 project_perspective(float fov_in_radians, unsigned width, unsigned height, float near_z, float far_z)
	{
		float aspect = width / static_cast<float>(height);
		VT_ASSERT(std::abs(aspect - std::numeric_limits<float>::epsilon()) > 0, "Bad aspect ratio.");
		float fov_tan = std::tan(fov_in_radians / 2);
		return {{
			{1 / (aspect * fov_tan), 0, 0, 0},
			{0, 1 / fov_tan, 0, 0},
			{0, 0, far_z / (far_z - near_z), 1},
			{0, 0, -(far_z * near_z) / (far_z - near_z), 0},
		}};
	}

	export Float4x4 look_at(Float3 const eye, Float3 const at, Float3 const up)
	{
		auto z_axis = normalize(at - eye);
		auto x_axis = normalize(cross(up, z_axis));
		auto y_axis = cross(z_axis, x_axis);
		return {{
			{x_axis.x, y_axis.x, z_axis.x, 0},
			{x_axis.y, y_axis.y, z_axis.y, 0},
			{x_axis.z, y_axis.z, z_axis.z, 0},
			{-dot(x_axis, eye), -dot(y_axis, eye), -dot(z_axis, eye), 1},
		}};
	}

	export Float4x4 translate(Float4x4 const& mat, Float3 const vec)
	{
		return {
			mat.rows[0],
			mat.rows[1],
			mat.rows[2],
			mat.rows[0] * vec.x + mat.rows[1] * vec.y + mat.rows[2] * vec.z + mat.rows[3],
		};
	}

	export Float4x4 rotate(Float4x4 const& mat, float const angle, Float3 const vec)
	{
		float const cos = std::cos(angle);
		float const sin = std::sin(angle);

		auto	 axis = normalize(vec);
		auto	 temp = (1 - cos) * axis;
		Float3x3 rotated {{
			{
				cos + temp.x * axis.x,
				temp.x * axis.y + sin * axis.z,
				temp.x * axis.z - sin * axis.y,
			},
			{
				temp.y * axis.x - sin * axis.z,
				cos + temp.y * axis.y,
				temp.y * axis.z + sin * axis.x,
			},
			{
				temp.z * axis.x + sin * axis.y,
				temp.z * axis.y - sin * axis.x,
				cos + temp.z * axis.z,
			},
		}};

		return {
			mat.rows[0] * rotated.rows[0].x + mat.rows[1] * rotated.rows[0].y + mat.rows[2] * rotated.rows[0].z,
			mat.rows[0] * rotated.rows[1].x + mat.rows[1] * rotated.rows[1].y + mat.rows[2] * rotated.rows[1].z,
			mat.rows[0] * rotated.rows[2].x + mat.rows[1] * rotated.rows[2].y + mat.rows[2] * rotated.rows[2].z,
			mat.rows[3],
		};
	}

	export Float4x4 scale(Float4x4 const& mat, Float3 const vec)
	{
		return {
			mat.rows[0] * vec.x,
			mat.rows[1] * vec.y,
			mat.rows[2] * vec.z,
			mat.rows[3],
		};
	}
}
