#include <gl\glew.h>
#include "Engine/GameMain.hpp"
#include "Engine/EventLoop.hpp"
#include "Engine/Tools/Log.hpp"
#include "OS\Scheduler.h"

int main()
{
	GameMain().run();

	return 0;
}