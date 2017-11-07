#ifndef GUI_MANAGER_H
#define GUI_MANAGER_H
#include "..\GraphicsManager.hpp"
#include "..\..\TimeManager.hpp"
#include "..\..\Sound\AudioManager.h"
#include "..\..\Engine\physics\BulletPhysicsManager.h"
#include "..\..\Engine\physics\BulletMoveableBody.h"

#include "Screens\IntroScreen.h"
#include "Screens\MainScreen1.h"
#include "Screens\GameScreen.h"
#include "Screens\LoadingScreen.h"


class GuiManager : public GraphicsComponent
{
public:
	GuiManager(GraphicsManager&, TimeManager&, InputManager& inputMan, AudioManager& soundMan);
	~GuiManager();

	void initialize();

protected:
	status load();
	void update();
	void draw();

private:
	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	InputManager& mInputManager;
	AudioManager& mSoundManager;
	PlainSprite mPlainSprite;

	BphysicsManager mBphysicsManager;

	IntroScreen* mIntroScreen;
	MainScreen* mMainScreen;
	GameScreen* mGameScreen;
	LoadingScreen* mLoadingScreen;

	typedef enum
	{
		SCREEN_0,
		SCREEN_1,
		SCREEN_2,
	} currentScreen_t;

	bool loadingScr;

	currentScreen_t mCurrentScreen;
};

#endif // !GUI_MANAGER_H