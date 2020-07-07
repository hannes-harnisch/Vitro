#pragma once

#include "TriangleLayer.h"

#include <Vitro.h>

class TriangleTest : public Vitro::Engine
{
public:
	TriangleTest() : Engine("", "")
	{
		AppWindow1 = Vitro::Window::New(550, 450, 300, 150, "TriangleTest 1");
		AppWindow2 = Vitro::Window::New(550, 450, 1000, 150, "TriangleTest 2");
	}

	void OnStart() override
	{
		AppWindow1->Attach<TriangleLayer>();
		AppWindow1->Open();

		AppWindow2->Attach<TriangleLayer>();
		AppWindow2->Open();
	}

private:
	Vitro::Ref<Vitro::Window> AppWindow1;
	Vitro::Ref<Vitro::Window> AppWindow2;
};
