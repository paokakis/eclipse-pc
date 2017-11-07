#include "EventLoop.hpp"
#include "Tools/Log.hpp"
#include "Portable.h"
#include "Input\Keyboard.h"

EventLoop::EventLoop(ActivityHandler& pActivityHandler, InputHandler& pInputHandler) :
	mEnabled(false), mQuit(false),
	mActivityHandler(pActivityHandler),
	mInputHandler(pInputHandler)
{}

void EventLoop::run()
{
	Log::info("Starting event loop");

	mActivityHandler.onStart();
	mActivityHandler.onCreateWindow();
	activate();
	mActivityHandler.onGainFocus();

	while (!mQuit)
	{
		processInputEvent();

		// Steps the application.
		if ((mEnabled) && (!mQuit))
		{
			if (mActivityHandler.onStep() != STATUS_OK || mInputHandler.mQuit)
			{
				mQuit = true;
			}
		}
	}

	mpScheduler->stop();
}

void EventLoop::activate()
{
	// Enables activity only if a window is available.
	if ((!mEnabled)) // && (mApplication->window != NULL))
	{

		mQuit = false;
		mEnabled = true;
		mpScheduler = Scheduler::getInstance();
		if (mActivityHandler.onActivate() != STATUS_OK)
		{
			goto _ERROR_;
		}
	}
	return;
_ERROR_:
	mQuit = true;
	deactivate();
}

void EventLoop::deactivate()
{
	if (mEnabled)
	{
		mActivityHandler.onDeactivate();
		mEnabled = false;
	}
}

void EventLoop::processAppEvent(int32_t pCommand)
{
	switch (pCommand)
	{
	case APP_CMD_CONFIG_CHANGED:
		mActivityHandler.onConfigurationChanged();
		break;
	case APP_CMD_INIT_WINDOW:
		mActivityHandler.onCreateWindow();
		break;
	case APP_CMD_DESTROY:
		mActivityHandler.onDestroy();
		break;
	case APP_CMD_GAINED_FOCUS:
		activate();
		mActivityHandler.onGainFocus();
		break;
	case APP_CMD_LOST_FOCUS:
		mActivityHandler.onLostFocus();
		deactivate();
		break;
	case APP_CMD_LOW_MEMORY:
		mActivityHandler.onLowMemory();
		break;
	case APP_CMD_PAUSE:
		mActivityHandler.onPause();
		deactivate();
		break;
	case APP_CMD_RESUME:
		mActivityHandler.onResume();
		break;
	case APP_CMD_SAVE_STATE:
		//mActivityHandler.onSaveInstanceState(&mApplication->savedState, &mApplication->savedStateSize);
		break;
	case APP_CMD_START:
		mActivityHandler.onStart();
		break;
	case APP_CMD_STOP:
		mActivityHandler.onStop();
		break;
	case APP_CMD_TERM_WINDOW:
		mActivityHandler.onDestroyWindow();
		deactivate();
		break;
	default:
		break;
	}
}

int32_t EventLoop::processInputEvent()
{
	if (!mEnabled) return 0;
	// Event processing loop.
	mInputHandler.onKeyboardEvent();
	mInputHandler.onMouseEvent();

	return 0;
}