module;
#include <cmath>
export module vt.Graphics.Camera;

import vt.Core.Matrix;
import vt.Core.Transform;
import vt.Core.Vector;

namespace vt
{
	export class Camera
	{
	public:
		Camera(Float3 position, Float3 target, Float4x4 const& projection) :
			projection(projection), position(position), forward(normalize(target))
		{
			update_view();
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
			update_view();
		}

		void translate(Float3 translation)
		{
			position += Float3x3 {right, up, forward} * translation;
			update_view();
		}

		void pitch(float radians)
		{
			forward = normalize(forward * std::cos(radians) + up * std::sin(radians));
			update_view();
		}

		void yaw(float radians)
		{
			forward = normalize(forward * std::cos(radians) + right * std::sin(radians));
			right	= cross(up, forward);
			update_view();
		}

		void roll(float radians)
		{
			right = normalize(right * std::cos(radians) + up * std::sin(radians));
			up	  = cross(forward, right);
			update_view();
		}

	private:
		static constexpr Float3 DefaultRight   = {1, 0, 0};
		static constexpr Float3 DefaultUp	   = {0, 1, 0};
		static constexpr Float3 DefaultForward = {0, 0, 1};

		Float4x4 projection;
		Float4x4 view_projection;
		Float3	 position;
		Float3	 right	 = DefaultRight;
		Float3	 up		 = DefaultUp;
		Float3	 forward = DefaultForward;

		void update_view()
		{
			view_projection = projection * look_at(position, position + forward, up);
		}
	};
}
