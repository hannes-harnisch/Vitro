#include "CubeTest/CubeTest.h"
#include "MeshTest/MeshTest.h"
#include "TextureTest/TextureTest.h"
#include "TriangleTest/TriangleTest.h"

int main(int argc, char** argv)
{
	auto app = new TextureTest();
	int exit = app->Run();
	delete app;
	return exit;
}
