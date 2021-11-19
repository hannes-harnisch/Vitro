export module vt.Graphics.Camera;

import vt.Core.Matrix;
import vt.Core.Vector;

namespace vt
{
	export class Camera
	{
	public:
		Camera(Float3 position, Float3 target, Float4x4 const& projection);

		Float4x4 const& get_projection() const;
		Float4x4 const& get_view_projection() const;
		Float3 const&	get_position() const;
		Float3 const&	right_direction() const;
		Float3 const&	up_direction() const;
		Float3 const&	forward_direction() const;
		void			set_position(Float3 pos);
		void			translate(Float3 translation);
		void			pitch(float radians);
		void			yaw(float radians);
		void			roll(float radians);

	private:
		static Float3 const DEFAULT_RIGHT; // TODO: wait for compiler fix, then change to constexpr
		static Float3 const DEFAULT_UP;
		static Float3 const DEFAULT_FORWARD;

		Float4x4 projection;
		Float4x4 view_projection;
		Float3	 position;
		Float3	 right	 = DEFAULT_RIGHT;
		Float3	 up		 = DEFAULT_UP;
		Float3	 forward = DEFAULT_FORWARD;

		void update_view_projection();
	};
}
