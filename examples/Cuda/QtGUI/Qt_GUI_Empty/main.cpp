#include <QtApp.h>
using namespace dyno;

int main()
{
	QtApp app;
	app.initialize(1366, 800);
	app.mainLoop();

	return 0;
}