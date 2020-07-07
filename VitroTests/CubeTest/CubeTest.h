#pragma once

#include "CubeLayer.h"

#include <Vitro.h>

class CubeTest : public Vitro::Engine
{
public:
	CubeTest() : Engine("", "")
	{
		AppWindow = Vitro::Window::New(1200, 900, 300, 150, "CubeTest");
	}

	void OnStart() override
	{
		AppWindow->Attach<CubeLayer>(AppWindow->GetWidth(), AppWindow->GetHeight());
		AppWindow->Open();
		AppWindow->TrapCursor(true);
	}

private:
	Vitro::Ref<Vitro::Window> AppWindow;
};
