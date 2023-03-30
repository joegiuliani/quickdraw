#include "quickdraw.h"

using namespace quickdraw;
using namespace window;

int main()
{
	if (!Init("Quickdraw Demo", 640, 480))
		return -1;

	shader::SetRectCornerMask(0);
	shader::SetOutlineColor(RGBA(0, 0, 0, 1));
	shader::SetOutlineThickness(1);
	shader::SetRectCornerMask(1);
	shader::SetRectCornerSize(5);

	while (!ShouldClose())
	{
		NewFrame();

		shader::SetFillColor(RGBA(1));
		DrawRect(Vec2(50), Vec2(100));

		shader::SetFillColor(RGBA(1,0,0,1));
		DrawRect(Vec2(500, 50), Vec2(50));

		shader::SetFillColor(RGBA(1, 0, 1, 1));
		DrawRect(Vec2(50, 300), Vec2(50));

		DrawFrame();
	}

	Terminate();

	return 0;
}