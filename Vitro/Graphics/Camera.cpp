module;
#include <cmath>
export module vt.Graphics.Camera;

import vt.Core.Matrix;
import vt.Core.Transform;
import vt.Core.Vector;
import vt.Trace.Log;

namespace vt
{
	export class Camera
	{
	public:
		Camera(Float3 position, Float3 target, Float4x4 const& projection) :
			projection(projection), position(position), forward(normalize(target))
		{
			update_view_projection();
		}

		Float4x4 const& get_projection() const
		{
			return projection;
		}

		Float4x4 const& get_view_projection() const
		{
			return view_projection;
		}

		Float3 const& get_position() const
		{
			return position;
		}

		Float3 const& right_direction() const
		{
			return right;
		}

		Float3 const& up_direction() const
		{
			return up;
		}

		Float3 const& forward_direction() const
		{
			return forward;
		}

		void set_position(Float3 pos)
		{
			position = pos;
			update_view_projection();
		}

		void translate(Float3 translation)
		{
			position += Float3x3 {right, up, forward} * translation;
			update_view_projection();
		}

		void pitch(float radians)
		{
			forward = normalize(forward * std::cos(radians) + up * std::sin(radians));
			update_view_projection();
		}

		void yaw(float radians)
		{
			forward = normalize(forward * std::cos(radians) + right * std::sin(radians));
			right	= cross(up, forward);
			update_view_projection();
		}

		void roll(float radians)
		{
			right = normalize(right * std::cos(radians) + up * std::sin(radians));
			up	  = cross(forward, right);
			update_view_projection();
		}

	private:
		static const inline Float3 DEFAULT_RIGHT   = {1, 0, 0}; // TODO: wait for compiler fix, then change to constexpr
		static const inline Float3 DEFAULT_UP	   = {0, 1, 0};
		static const inline Float3 DEFAULT_FORWARD = {0, 0, 1};

		Float4x4 projection;
		Float4x4 view_projection;
		Float3	 position;
		Float3	 right	 = DEFAULT_RIGHT;
		Float3	 up		 = DEFAULT_UP;
		Float3	 forward = DEFAULT_FORWARD;

		void update_view_projection()
		{
			view_projection = look_at(position, position + forward, up) * projection;
		}
	};
}
