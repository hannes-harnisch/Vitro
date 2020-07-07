#pragma once

#include "TextureLayer.h"

#include <Vitro.h>

class TextureTest : public Vitro::Engine
{
public:
	TextureTest() : Engine("", "")
	{
		AppWindow = Vitro::Window::New(1200, 900, 300, 150, "CubeTest");
	}

	void OnStart() override
	{
		AppWindow->Attach<TextureLayer>(AppWindow->GetWidth(), AppWindow->GetHeight());
		AppWindow->Open();
		AppWindow->TrapCursor(true);
	}

private:
	Vitro::Ref<Vitro::Window> AppWindow;
};
