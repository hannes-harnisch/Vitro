module;
#include <cmath>
module vt.Graphics.Camera;

import vt.Core.Matrix;
import vt.Core.Transform;
import vt.Core.Vector;
import vt.Trace.Log;

namespace vt
{
	Float3 const Camera::DEFAULT_RIGHT	 = {1, 0, 0}; // TODO: wait for compiler fix, then change to constexpr
	Float3 const Camera::DEFAULT_UP		 = {0, 1, 0};
	Float3 const Camera::DEFAULT_FORWARD = {0, 0, 1};

	Camera::Camera(Float3 position, Float3 target, Float4x4 const& projection) :
		projection(projection), position(position), forward(normalize(target))
	{
		update_view_projection();
	}

	Float4x4 const& Camera::get_projection() const
	{
		return projection;
	}

	Float4x4 const& Camera::get_view_projection() const
	{
		return view_projection;
	}

	Float3 const& Camera::get_position() const
	{
		return position;
	}

	Float3 const& Camera::right_direction() const
	{
		return right;
	}

	Float3 const& Camera::up_direction() const
	{
		return up;
	}

	Float3 const& Camera::forward_direction() const
	{
		return forward;
	}

	void Camera::set_position(Float3 pos)
	{
		position = pos;
		update_view_projection();
	}

	void Camera::translate(Float3 translation)
	{
		position += Float3x3 {right, up, forward} * translation;
		update_view_projection();
	}

	void Camera::pitch(float radians)
	{
		forward = normalize(forward * std::cos(radians) + up * std::sin(radians));
		update_view_projection();
	}

	void Camera::yaw(float radians)
	{
		forward = normalize(forward * std::cos(radians) + right * std::sin(radians));
		right	= cross(up, forward);
		update_view_projection();
	}

	void Camera::roll(float radians)
	{
		right = normalize(right * std::cos(radians) + up * std::sin(radians));
		up	  = cross(forward, right);
		update_view_projection();
	}

	void Camera::update_view_projection()
	{
		view_projection = look_at(position, position + forward, up) * projection;
	}
}
