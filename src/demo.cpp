#include "quickdraw.h"
#include <vector>
using namespace quickdraw;
using namespace window;

int main()
{
	if (!Init("Quickdraw Demo", 640, 480))
		return -1;

	shader::SetRectCornerMask(0);
	shader::SetOutlineColor(RGBA(0, 0, 0, 1));

	shader::SetOutlineThickness(2);

	shader::SetFillColor(RGBA(0,0,0,1), shader::TOP_LEFT);
	shader::SetFillColor(RGBA(1,0,0,1), shader::TOP_RIGHT);
	shader::SetFillColor(RGBA(1,0,1,1), shader::BOTTOM_RIGHT);
	shader::SetFillColor(RGBA(0,0,1,1), shader::BOTTOM_LEFT);


	std::vector<Vec2> path_verts;
	path_verts.push_back(Vec2(50,50));
	path_verts.push_back(Vec2(50, 100));
	path_verts.push_back(Vec2(100, 100));


	while (!ShouldClose())
	{
		NewFrame();

		DrawPath(path_verts, 3, Vec2(0));

		DrawFrame();
	}

	Terminate();

	return 0;
}