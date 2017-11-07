#include "GameMain.hpp"
#include "Tools/Log.hpp"
#include "../Game/common/MapCoordinates.h"
#include "../Engine/Graphics/Configuration/Folders.h"


GameMain::GameMain() :
	mpTimeManager(TimeManager::getInstance()),
	mGraphicsManager(*mpTimeManager),
	mInputManager(mGraphicsManager),
	mSoundManager(mGraphicsManager),
	mEventLoop(*this, mInputManager),
	mBGM(BGM_MUSIC),
	mGuiManager(mGraphicsManager, *mpTimeManager, mInputManager, mSoundManager)
{
	Log::info("Creating DroidBlaster");
}

void GameMain::run() 
{
	mEventLoop.run();
}

status GameMain::onActivate() 
{
	Log::info("GameMain onActivate");
	if (mGraphicsManager.start() != STATUS_OK) 
		return STATUS_KO;
	if (mSoundManager.start(WORLD_DIMENS) != STATUS_OK)
		return STATUS_KO;
	mInputManager.start();
	mpTimeManager->reset();
	mEventManager = EventManager::getInstance();
	mEventManager->start();

	mSoundManager.setListener(
		mGraphicsManager.getCamPosition(), 
		glm::vec3(0), 
		mGraphicsManager.getCamTarget(), 
		mGraphicsManager.getCamUp());

	mSoundManager.playBGM(mBGM);

	mGuiManager.initialize();
	
	return STATUS_OK;
}

void GameMain::onDeactivate() 
{
	Log::info("GameMain onDeactivate");
	mGraphicsManager.stop();
	mSoundManager.stop();
}

status GameMain::onStep() 
{
	glfwPollEvents();
	mpTimeManager->update();
	mInputManager.update();

	// TODO check if this setting needs to be updated constantly
	mSoundManager.setListener(
		mGraphicsManager.getCamPosition(),
		glm::vec3(0),
		mGraphicsManager.getCamTarget(),
		mGraphicsManager.getCamUp());

	return mGraphicsManager.update();
}

void GameMain::onStart() 
{
	Log::warn("GameMain onStart UNHANDLED!!!!");
}

void GameMain::onResume() 
{
	Log::warn("GameMain onResume UNHANDLED!!!!");
}

void GameMain::onPause() 
{ 
	Log::warn("GameMain onPause UNHANDLED!!!!");
}

void GameMain::onStop()
{ 
	Log::warn("GameMain onStop UNHANDLED!!!!");
}

void GameMain::onDestroy() 
{ 
	Log::warn("GameMain onDestroy UNHANDLED!!!!");
}