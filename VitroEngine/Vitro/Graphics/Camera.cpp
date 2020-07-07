#include "_pch.h"
#include "Camera.h"

namespace Vitro
{
	Camera::Camera(const Float3& position, const Float3& target, const Float4x4& projection) :
		Projection(projection), Position(position), Forward(Normalize(target))
	{
		UpdateView();
	}

	Float4x4 Camera::GetView() const
	{
		return View;
	}

	Float4x4 Camera::GetProjection() const
	{
		return Projection;
	}

	Float4x4 Camera::GetViewProjection() const
	{
		return Projection * View;
	}

	Float3 Camera::GetPosition() const
	{
		return Position;
	}

	void Camera::SetPosition(const Float3& position)
	{
		Position = position;
		UpdateView();
	}

	void Camera::Translate(const Float3& translation)
	{
		Position += Float3x3(Right, Up, Forward) * translation;
		UpdateView();
	}

	void Camera::Pitch(float radians)
	{
		Forward = Normalize(Forward * std::cos(radians) + Up * std::sin(radians));
		UpdateView();
	}

	void Camera::Yaw(float radians)
	{
		Forward = Normalize(Forward * std::cos(radians) + Right * std::sin(radians));
		Right	= Cross(Up, Forward);
		UpdateView();
	}

	void Camera::Roll(float radians)
	{
		Right = Normalize(Right * std::cos(radians) + Up * std::sin(radians));
		Up	  = Cross(Forward, Right);
		UpdateView();
	}

	Float3 Camera::RightDirection() const
	{
		return Right;
	}

	Float3 Camera::UpDirection() const
	{
		return Up;
	}

	Float3 Camera::ForwardDirection() const
	{
		return Forward;
	}

	void Camera::UpdateView()
	{
		View = LookAt(Position, Position + Forward, Up);
	}
}
