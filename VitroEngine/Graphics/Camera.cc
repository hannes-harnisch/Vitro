module;
#include <cmath>
export module Vitro.Graphics.Camera;

import Vitro.Core.Matrix;
import Vitro.Core.Transform;
import Vitro.Core.Vector;

namespace vt
{
	export class Camera
	{
	public:
		Camera(Float3 position, Float3 target, Float4x4 const& projection) :
			projection(projection), position(position), forward(normalize(target))
		{
			updateView();
		}

		Float4x4 const& getProjection() const
		{
			return projection;
		}

		Float4x4 const& getViewProjection() const
		{
			return viewProjection;
		}

		Float3 const& getPosition() const
		{
			return position;
		}

		Float3 const& rightDirection() const
		{
			return right;
		}

		Float3 const& upDirection() const
		{
			return up;
		}

		Float3 const& forwardDirection() const
		{
			return forward;
		}

		void setPosition(Float3 pos)
		{
			position = pos;
			updateView();
		}

		void translate(Float3 translation)
		{
			position += Float3x3 {right, up, forward} * translation;
			updateView();
		}

		void pitch(float radians)
		{
			forward = normalize(forward * std::cos(radians) + up * std::sin(radians));
			updateView();
		}

		void yaw(float radians)
		{
			forward = normalize(forward * std::cos(radians) + right * std::sin(radians));
			right	= cross(up, forward);
			updateView();
		}

		void roll(float radians)
		{
			right = normalize(right * std::cos(radians) + up * std::sin(radians));
			up	  = cross(forward, right);
			updateView();
		}

	private:
		static constexpr Float3 DefaultRight   = {1, 0, 0};
		static constexpr Float3 DefaultUp	   = {0, 1, 0};
		static constexpr Float3 DefaultForward = {0, 0, 1};

		Float4x4 projection		= {};
		Float4x4 viewProjection = {};
		Float3	 position		= {};
		Float3	 right			= DefaultRight;
		Float3	 up				= DefaultUp;
		Float3	 forward		= DefaultForward;

		void updateView()
		{
			viewProjection = projection * lookAt(position, position + forward, up);
		}
	};
}
