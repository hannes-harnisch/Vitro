module;
#include "Trace/Assert.hh"

#include <cmath>
#include <limits>
export module Vitro.Core.Transform;

import Vitro.Core.Matrix;
import Vitro.Core.Vector;

namespace vt
{
	export Float4x4 projectOrthographic(float left, float right, float bottom, float top, float nearZ, float farZ)
	{
		return {{
			{2 / (right - left), 0, 0, 0},
			{0, 2 / (top - bottom), 0, 0},
			{0, 0, 2 / (farZ - nearZ), 0},
			{-(right + left) / (right - left), -(top + bottom) / (top - bottom), -nearZ / (farZ - nearZ), 1},
		}};
	}

	export Float4x4 projectPerspective(float fovInRadians, unsigned width, unsigned height, float nearZ, float farZ)
	{
		float aspect = width / static_cast<float>(height);
		vtAssert(std::abs(aspect - std::numeric_limits<float>::epsilon()) > 0, "Bad aspect ratio.");
		float fovTan = std::tan(fovInRadians / 2);
		return {{
			{1 / (aspect * fovTan), 0, 0, 0},
			{0, 1 / fovTan, 0, 0},
			{0, 0, farZ / (farZ - nearZ), 1},
			{0, 0, -(farZ * nearZ) / (farZ - nearZ), 0},
		}};
	}

	export Float4x4 lookAt(Float3 const eye, Float3 const at, Float3 const up)
	{
		auto zAxis = normalize(at - eye);
		auto xAxis = normalize(cross(up, zAxis));
		auto yAxis = cross(zAxis, xAxis);
		return {{
			{xAxis.x, yAxis.x, zAxis.x, 0},
			{xAxis.y, yAxis.y, zAxis.y, 0},
			{xAxis.z, yAxis.z, zAxis.z, 0},
			{-dot(xAxis, eye), -dot(yAxis, eye), -dot(zAxis, eye), 1},
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

		auto axis = normalize(vec);
		auto temp = (1 - cos) * axis;
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
