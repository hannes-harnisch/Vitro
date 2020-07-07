#pragma once

#include "Vitro/Graphics/IndexBuffer.h"
#include "Vitro/Math/Vector.h"
#include "Vitro/Utility/Ref.h"
#include "Vitro/Utility/Scope.h"

namespace Vitro
{
	class Context3D
	{
	public:
		static Scope<Context3D> New(void* nativeWindowHandle, uint32_t width, uint32_t height);

		Context3D() = default;

		virtual ~Context3D() = default;

		virtual void SetViewport(uint32_t width, uint32_t height, int x, int y) = 0;
		virtual void SetClearColor(const Float4& color)							= 0;
		virtual void SetVSync(bool enabled)										= 0;
		virtual void TargetBackBuffer()											= 0;
		virtual void DrawIndices(const Ref<IndexBuffer>& ib)					= 0;
		virtual void SwapBuffers()												= 0;

		uint32_t GetWidth() const
		{
			return Width;
		}
		uint32_t GetHeight() const
		{
			return Height;
		}

		Context3D(const Context3D&) = delete;
		Context3D& operator=(const Context3D&) = delete;

	protected:
		uint32_t Width	= 0;
		uint32_t Height = 0;
	};
}
