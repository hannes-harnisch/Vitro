#include "_pch.h"
#include "Renderer3D.h"

#include "Vitro/Client/Window.h"

namespace Vitro
{
	Renderer3D::Renderer3D(void* nativeWindowHandle, uint32_t width, uint32_t height)
	{
		Context = Context3D::New(nativeWindowHandle, width, height);
		Context->SetViewport(width, height, 0, 0);
	}

	Scope<Context3D>& Renderer3D::GetContext()
	{
		return Context;
	}

	void Renderer3D::BeginScene()
	{
		Context->SetClearColor({0.1, 0.1, 0.1, 1});
		Context->TargetBackBuffer();
	}

	void Renderer3D::EndScene()
	{
		Context->SwapBuffers();
	}

	void Renderer3D::Submit(const Ref<IndexBuffer>& ib)
	{
		ib->Bind();
		Context->DrawIndices(ib);
	}
}
