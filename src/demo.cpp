#include "../include/quickdraw/quickdraw.h"
#include <vector>
using namespace quickdraw;
int main()
{
	if (!Init("Quickdraw Demo", Vec2(640,480)))
		return -1;
	SetFillColor(RGBA(1, 0, 0, 1), UPPER_START);
	SetFillColor(RGBA(0, 1, 1, 1), UPPER_END);
	SetFillColor(RGBA(0, 1, 0, 1), LOWER_START);
	SetFillColor(RGBA(1, 0, 1, 1), LOWER_END);
	Texture* texture = LoadTexture("C:/Users/Joe/Desktop/stackoverflowquestion.PNG").first;
	SetTextScale(40);
	std::vector<Vec2> spiral;
	spiral.resize(500);
	for (int ti = 0; ti < 500; ti++)
	{
		float t = 2 * 3.14159265358 * (float)ti / 500;
		spiral[ti] = Vec2(100 * (float(ti)/500) *cosf(t), 50.0f * sinf(t) + 100 * (float(ti) / 500));
	}

	SetBackgroundColor(RGBA(0.2, 0.2, 0.2, 1));
	while (!ShouldClose())
	{
		NewFrame();
		SetFillColor(RGBA(1, 0, 0, 1), UPPER_START);
		SetFillColor(RGBA(1, 1, 1, 1), UPPER_END);
		SetFillColor(RGBA(0, 0, 0, 1), LOWER_START);
		SetFillColor(RGBA(0, 0, 0, 1), LOWER_END);
	    DrawText(Vec2(200, 200), "Quickdraw");

		// Rect 1
		SetFillColor(RGBA(1, 0, 0, 1), UPPER_START);
		SetFillColor(RGBA(0, 1, 1, 1), UPPER_END);
		SetFillColor(RGBA(0, 1, 0, 1), LOWER_START);
		SetFillColor(RGBA(1, 0, 1, 1), LOWER_END);
		SetOutlineColor(RGBA(1));
		SetRectRoundedSize(10);
		SetRectRoundedMask(1);
		SetRectRoundedMask(0, LOWER_START);
		SetRectRoundedMask(0, LOWER_END);
		SetOutlineThickness(2);
		DrawRect(Vec2(0, 0), Vec2(100));

		// Rect 2
		SetFillColor(RGBA(0.5, 0, 0, 0.5), UPPER_START);
		SetFillColor(RGBA(0.5, 0, 0, 0.5), UPPER_END);
		SetFillColor(RGBA(0.5, 0.5, 0.5, 0.5), LOWER_START);
		SetFillColor(RGBA(0.5, 0.5, 0.5, 0.5), LOWER_END);
		SetOutlineColor(RGBA(0.1, 0.1, 1, 1), UPPER_START);
		SetOutlineColor(RGBA(0.1, 0.1, 1, 1), UPPER_END);
		SetOutlineColor(RGBA(0.1, 0.1, 1, 1), LOWER_START);
		SetOutlineColor(RGBA(1, 1, 1, 1), LOWER_END);
		int radius = 50;
		SetRectRoundedSize(radius);
		DrawRect(Vec2(75, 0), Vec2(radius * 2));

        // Path
		SetFillColor(RGBA(0, 0, 0, 1), UPPER_START);
		SetFillColor(RGBA(0, 0, 1, 0), UPPER_END);
		SetFillColor(RGBA(1, 1, 1, 1), LOWER_START);
		SetFillColor(RGBA(0, 0, 1, 0), LOWER_END);
		SetOutlineThickness(1);
		SetOutlineColor(RGBA(0, 1, 1, 1), UPPER_START);
		SetOutlineColor(RGBA(0, 0, 1, 1), UPPER_END);
		SetOutlineColor(RGBA(1, 0, 1, 1), LOWER_START);
		SetOutlineColor(RGBA(1, 0, 0, 1), LOWER_END);
		DrawPath(spiral, 6, Vec2(300, 300));

		DrawFrame();
	}
	Terminate();

	return 0;
}