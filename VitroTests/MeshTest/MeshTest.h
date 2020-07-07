#pragma once

#include "MeshLayer.h"

#include <Vitro.h>

class MeshTest : public Vitro::Engine
{
public:
	MeshTest() : Engine("", "")
	{
		AppWindow = Vitro::Window::New(1200, 900, 300, 150, "CubeTest");
	}

	void OnStart() override
	{
		AppWindow->Attach<MeshLayer>(AppWindow->GetWidth(), AppWindow->GetHeight());
		AppWindow->Attach<Vitro::Editor>();
		AppWindow->Open();
		AppWindow->TrapCursor(true);
	}

private:
	Vitro::Ref<Vitro::Window> AppWindow;
};
