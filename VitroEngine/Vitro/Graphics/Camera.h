#pragma once

#include "Vitro/Math/Matrix.h"
#include "Vitro/Math/Vector.h"

namespace Vitro
{
	class Camera final
	{
	public:
		Camera() = default;
		Camera(const Float3& position, const Float3& target, const Float4x4& projection);

		Float4x4 GetView() const;
		Float4x4 GetProjection() const;
		Float4x4 GetViewProjection() const;
		Float3 GetPosition() const;
		void SetPosition(const Float3& position);
		void Translate(const Float3& translation);
		void Pitch(float radians);
		void Yaw(float radians);
		void Roll(float radians);
		Float3 RightDirection() const;
		Float3 UpDirection() const;
		Float3 ForwardDirection() const;

	private:
		static constexpr Float3 DefaultRight {1, 0, 0};
		static constexpr Float3 DefaultUp {0, 1, 0};
		static constexpr Float3 DefaultForward {0, 0, 1};

		Float4x4 Projection, View;
		Float3 Position;
		Float3 Right   = DefaultRight;
		Float3 Up	   = DefaultUp;
		Float3 Forward = DefaultForward;

		void UpdateView();
	};
}
