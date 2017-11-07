#include "GuiManager.h"
#include "..\..\tools\Log.hpp"

int subSteps = -1;

GuiManager::GuiManager(GraphicsManager& graphMan, TimeManager& timeMan, InputManager& inputMan, AudioManager& soundMan) :
	mGraphicsManager(graphMan), mTimeManager(timeMan), mInputManager(inputMan), mSoundManager(soundMan), mPlainSprite(mGraphicsManager, mTimeManager),
	mBphysicsManager(graphMan, timeMan), mCurrentScreen(SCREEN_0)
{
	Log::info("GuiManager constructor");
	
	mIntroScreen = new IntroScreen(graphMan, timeMan);
	mLoadingScreen = new LoadingScreen(graphMan, timeMan, mPlainSprite);

	mGraphicsManager.registerComponent(this, 1);
}

GuiManager::~GuiManager()
{
	Log::info("GuiManager destructor");
}

status GuiManager::load()
{
	Log::info("GuiManager load");

	mIntroScreen->screenLoad();
	mLoadingScreen->screenLoad();

	mGraphicsManager.glErrorCheck();

	return STATUS_OK;
}

void GuiManager::update()
{
	if (mCurrentScreen == SCREEN_0 && !loadingScr) mIntroScreen->screenUpdate();
	else if (mCurrentScreen == SCREEN_1 && !loadingScr) mMainScreen->screenUpdate();
	else if (mCurrentScreen == SCREEN_2 && !loadingScr) mGameScreen->screenUpdate();
	else;
	if (loadingScr) mLoadingScreen->screenUpdate();

	if (mCurrentScreen == SCREEN_0 && mIntroScreen->screenDrawFinished())
	{
		if (subSteps == -1)
		{
			loadingScr = true;
			mLoadingScreen->setProgress(10);
			subSteps++;
		}
		else if (subSteps == 0)
		{
			mMainScreen = new MainScreen(mGraphicsManager, mTimeManager, mInputManager, mBphysicsManager);
			mLoadingScreen->setProgress(50);
			subSteps++;
		}
		else if (subSteps == 1)
		{
			mMainScreen->screenLoad();
			mLoadingScreen->setProgress(100);
			subSteps++;
		}
		else if (subSteps == 2)
		{
			mMainScreen->initialize();
			subSteps++;
		}
		else if (subSteps == 3)
		{
			mCurrentScreen = SCREEN_1;
			loadingScr = false;
			delete mIntroScreen;
			subSteps = -1;
			mLoadingScreen->setProgress(0);
		}
	}
	else if (mCurrentScreen == SCREEN_1 && mMainScreen != NULL && mMainScreen->screenDrawFinished())
	{
		if (subSteps == -1)
		{
			loadingScr = true;
			mLoadingScreen->setProgress(10);
			subSteps++;
		}
		else if (subSteps == 0)
		{
			mGameScreen = new GameScreen(mGraphicsManager, mTimeManager, mInputManager, mSoundManager, mBphysicsManager, mPlainSprite);
			mLoadingScreen->setProgress(50);
			subSteps++;
		}
		else if (subSteps == 1)
		{
			mGameScreen->screenLoad();
			mLoadingScreen->setProgress(100);
			subSteps++;
		}
		else if (subSteps == 2)
		{
			mGameScreen->initialize();
			subSteps++;
		}
		else if (subSteps == 3)
		{
			mInputManager.setMouseShow(false);
			mInputManager.setMouseLocked(false);
			mGraphicsManager.setCamMouseMove(true);
			delete mMainScreen;
			mCurrentScreen = SCREEN_2;
			loadingScr = false;
			subSteps = -1;
		}
	}
	else;
}

void GuiManager::draw()
{
	if (mCurrentScreen == SCREEN_0 && !loadingScr)
	{
		mIntroScreen->screenDraw();
	}
	else if (mCurrentScreen == SCREEN_1 && !loadingScr)
	{
		mMainScreen->screenDraw();
	}
	else if (mCurrentScreen == SCREEN_2 && !loadingScr)
	{
		mGameScreen->screenDraw();
	}
	if (loadingScr)
	{
		mLoadingScreen->screenDraw();
	}
}

void GuiManager::initialize()
{
	mPlainSprite.initialize();

	mIntroScreen->initialize();
	mLoadingScreen->initialize();
	mBphysicsManager.start();

	loadingScr = false;
}