#ifndef GAMEMAIN_HPP
#define GAMEMAIN_HPP

#include "ActivityHandler.hpp"
#include "EventLoop.hpp"
#include "Input/InputManager.hpp"
#include "Graphics/GraphicsManager.hpp"
#include "Graphics\GUI\GuiManager.h"
#include "Resource.hpp"
#include "TimeManager.hpp"
#include "Tools/Types.hpp"
#include "Tools/Log.hpp"
#include "Sound\AudioManager.h"
#include "Graphics\GUI\EventManager.hpp"


class GameMain : public ActivityHandler {
public:
	GameMain();
	void run();
protected:
	status onActivate();
	void onDeactivate();
	status onStep();
	void onStart();

	void onResume();
	void onPause();
	void onStop();
	void onDestroy();

private:
	TimeManager* mpTimeManager;
	GraphicsManager mGraphicsManager;
	InputManager mInputManager;
	GuiManager mGuiManager;
	AudioManager mSoundManager;
	EventLoop mEventLoop;
	EventManager* mEventManager;

	std::string mBGM;
};

#endif
