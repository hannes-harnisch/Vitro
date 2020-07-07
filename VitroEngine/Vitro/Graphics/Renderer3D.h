#pragma once

#include "Vitro/Graphics/Context3D.h"
#include "Vitro/Graphics/VertexBuffer.h"
#include "Vitro/Utility/Ref.h"

namespace Vitro
{
	class Renderer3D final : public RefCounted
	{
	public:
		Renderer3D(void* nativeWindowHandle, uint32_t width, uint32_t height);
		virtual ~Renderer3D() = default;

		Scope<Context3D>& GetContext();
		void BeginScene();
		void EndScene();
		void Submit(const Ref<IndexBuffer>& ib);

	private:
		Scope<Context3D> Context;
	};
}
