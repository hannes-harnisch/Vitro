module;
#include <numbers>
export module vt.Core.Transform;

import vt.Core.Matrix;
import vt.Core.Rect;
import vt.Core.Vector;

namespace vt
{
	export Float4x4 project_orthographic(float left, float right, float bottom, float top, float near_z, float far_z);
	export Float4x4 project_perspective(float fov_in_radians, Extent size, float near_z, float far_z);
	export Float4x4 look_at(Float3 const eye, Float3 const at, Float3 const up);
	export Float4x4 translate(Float4x4 const& mat, Float3 const vec);
	export Float4x4 rotate(Float4x4 const& mat, float const angle, Float3 const vec);
	export Float4x4 scale(Float4x4 const& mat, Float3 const vec);

	export constexpr auto degrees(auto radians)
	{
		return 180.0f / std::numbers::pi_v<float> * radians;
	}

	export constexpr auto radians(auto degrees)
	{
		return degrees / (180.0f / std::numbers::pi_v<float>);
	}
}
