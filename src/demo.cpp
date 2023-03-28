#include "quickdraw.h"

using namespace quickdraw;
using namespace window;

int main()
{
	if (!Init("Quickdraw Demo", 640, 480))
		return -1;

	while (!ShouldClose())
	{
		NewFrame();

		shader::SetFillColor(RGBA(1));
		DrawRect(Vec2(50), Vec2(100));

		Draw();
	}

	Terminate();

	return 0;
}