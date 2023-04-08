#include "include/quickdraw/quickdraw.h"

using namespace quickdraw;

int main()
{
	Init("Quickdraw", Vec2(640, 480));

	while (!ShouldClose())
	{
		NewFrame();
		DrawFrame();
	}

	Terminate();
	return 0;
}