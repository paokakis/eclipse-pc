#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

#include "ActivityHandler.hpp"
#include "Input/InputHandler.hpp"
#include "../OS/Scheduler.h"

class EventLoop {
public:
	EventLoop(ActivityHandler& pActivityHandler, InputHandler& pInputHandler);
	void run();

private:
	void activate();
	void deactivate();

	void processAppEvent(int32_t pCommand);
	int32_t processInputEvent();
private:
	bool mEnabled;
	bool mQuit;
	ActivityHandler& mActivityHandler;
	InputHandler& mInputHandler;
	Scheduler* mpScheduler;
};

#endif