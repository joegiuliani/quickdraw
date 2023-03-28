#include "quickdraw.h"

using namespace quickdraw;

class Runner : public window::FrameObserver
{
	virtual void on_new_frame()
	{
		window::shader::SetFill(Color(1), Color(0));
		window::DrawRect(Vec2(20,20), Vec2(100));
	}
};

int main()
{
	window::Init("Quickdraw Demo", 640, 480);

	Runner r;
	window::AddFrameObserver(&r);
	
	window::Run();
}