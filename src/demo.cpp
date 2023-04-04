#include "../include/quickdraw/quickdraw.h"
#include <vector>
using namespace quickdraw;
using namespace window;

int main()
{
	if (!Init("Quickdraw Demo", 640, 480))
		return -1;

	shader::SetTextScale(48);

	while (!ShouldClose())
	{
		NewFrame();

		DrawText(Vec2(0	, 400), "Hello World!");

		DrawFrame();
	}
	Terminate();

	return 0;
}