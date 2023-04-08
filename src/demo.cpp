#include "quickdraw.h"

using namespace quickdraw;

int main()
{
	Init("QuickGUI", Vec2(640, 480));
	SetBackgroundColor(RGBA(0.25, 0.25, 0.25, 1));
	SetOutlineColor(RGBA(0.0,0.6353,0.9294,1));
	SetOutlineColor(RGBA(0.2, 0.2, 0.2, 1), LOWER_START);
	SetOutlineColor(RGBA(0.2, 0.2, 0.2, 1), LOWER_END);
	SetOutlineThickness(3);
	SetRectRoundedSize(10);
	SetRectRoundedMask(1);
	SetTextSpacing(0.001);
	while (!ShouldClose())
	{
		SetTextScale(24 + sinf(Time())*10);
		NewFrame();
		SetFillColor(RGBA(0, 0, 0, 1));
		DrawRect(Vec2(0), Vec2(100));
		SetFillColor(RGBA(1));
		DrawText(Vec2(10,20), "Quickdraw");
		DrawFrame();
	}

	Terminate();
	return 0;
}