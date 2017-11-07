#ifndef GAME_SCREEN_H
#define GAME_SCREEN_H
#include "..\..\GraphicsManager.hpp"
#include "..\..\..\Engine\TimeManager.hpp"
#include "..\..\Text2D.h"
#include "..\..\Engine\Sound\AudioManager.h"

#include "..\..\Engine\physics\BulletPhysicsManager.h"
#include "..\..\Engine\physics\BulletMoveableBody.h"

#include "..\..\Engine\Input\InputManager.hpp"
#include "..\..\Engine\Graphics\PlainSprite.h"
#include "..\..\Engine\Graphics\SpriteBatch.h"

#include "..\..\Game\Levels\Level1\GameObjects\SpaceShip.h"
#include "..\..\Game\Levels\Level1\GameObjects\BackGround.h"
#include "..\..\Game\common\effects\HQ\Sky.h"
#include "..\..\Game\Levels\Level1\GameObjects\Asteroid.hpp"
#include "..\..\Game\Levels\Level1\GameObjects\Planet.h"
#include "..\..\Game\Levels\Level1\GameObjects\StarField.hpp"
#include "..\..\Game\Levels\Level1\GameObjects\LandScape.h"
#include "..\Engine\Graphics\GUI\Primitives\Primitives.h"
#include "..\Engine\Graphics\SkyBox.h"
#include <vector>

class GameScreen
{
public:
	GameScreen(GraphicsManager& graphMan, TimeManager& timeMan, InputManager& inputMan, 
		AudioManager& soundMan, BphysicsManager& mBphysics, PlainSprite& plainSprite);
	~GameScreen();

	void initialize();

	status screenLoad();
	void screenUpdate();
	void screenDraw();
	bool screenDrawFinished();

private:
	struct Vertex {
		GLfloat x, y, z;
	};
	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	InputManager& mInputManager;
	AudioManager& mSoundManager;
	BphysicsManager& mBphysicsManager;	
	PlainSprite& mPlainSprite;
	Primitives mPrimitive;

	SpriteBatch mSpriteBatch;
	Text2D mText2D;
	std::string mText;

	SpaceShip mSpaceShip;
	BackGround mBackGround;
	Sky mSky;
	//SkyBox* mSky;
	Asteroid mAsteroids;
	Planet mPlanetEarth;
	Planet mPlanetMoon;
	Planet mPlanetMars;
	Planet mPlanetJupiter;
	Planet mPlanetSaturn;
	Planet mPlanetSun;
	Planet mPlanetMercury;
	Planet mPlanetVenus;
	Planet mPlanetUranus;
	Planet mPlanetNeptune;
	//LandScape mLandScape;

	void prepVrtx(Vertex pVertices[4]);

	std::vector<GLushort> mIndexes;
	std::vector<Vertex> mVertices;

	GLuint mShaderProgram, aPosition, uProjection, uColor;

	glm::vec4 mTextColor;
	glm::vec3 mTextPos;
	glm::vec4 mColor;
	glm::mat4 mProjection;
	glm::vec3 mCameraPos;
	glm::vec3 mCamFocusPos;
};

#endif