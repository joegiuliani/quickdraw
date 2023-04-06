#include "../include/quickdraw/quickdraw.h"
#include <vector>
using namespace quickdraw;


int main()
{
	if (!Init("Quickdraw Demo", Vec2(640,480)))
		return -1;
	SetFillColor(RGBA(255, 0, 0, 255), UPPER_START);
	SetFillColor(RGBA(0, 255, 255, 255), UPPER_END);
	SetFillColor(RGBA(0, 255, 0, 255), LOWER_START);
	SetFillColor(RGBA(255, 0, 255, 255), LOWER_END);
	Texture* texture = LoadTexture("C:/Users/Joe/Desktop/stackoverflowquestion.PNG").first;

	std::vector<Vec2> spiral;
	spiral.resize(500);
	for (int ti = 0; ti < 500; ti++)
	{
		float t = 2 * 3.14159265358 * (float)ti / 500;
		spiral[ti] = Vec2(100 * (float(ti)/500) *cosf(t), 50.0f * sinf(t) + 100 * (float(ti) / 500));
	}

	while (!ShouldClose())
	{
		NewFrame();
		//DrawTexture(texture, Vec2(0, 0), Vec2(385, 386));
	    DrawText(Vec2(200, 200), "Textures");
		DrawPath(spiral, 2, Vec2(300, 300));
		DrawFrame();
	}
	Terminate();

	return 0;
}