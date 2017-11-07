#include "GameScreen.h"
#include "..\..\Engine\tools\Log.hpp"
#include "../Game/common/MapCoordinates.h"
#include "../Engine/Graphics/Configuration/Folders.h"
#include "../Engine/Graphics/Configuration/Colors.h"

static const char* VERTEX_SHADER =
"#version 150 core\n"
"precision highp float;\n"
"attribute vec3 aPosition;\n"
"uniform mat4 uProjection;\n"
"void main() {\n"
" gl_Position = uProjection * vec4(aPosition, 1.0 );\n"
"}";

static const char* FRAGMENT_SHADER =
"#version 150 core\n"
"precision highp float;\n"
"uniform vec4 uColor;\n"
"void main() {\n"
" gl_FragColor = uColor;\n"
"}\n";

static int32_t nbFrames = 0;

float primObj[] = { 0.7, -0.6, 0.0f, 1.0f, 0.0f, 32.0f };

GameScreen::GameScreen(GraphicsManager& graphMan, TimeManager& timeMan, InputManager& inputMan, AudioManager& soundMan, BphysicsManager& mBphysics, PlainSprite& plainSprite) :
	mGraphicsManager(graphMan), mTimeManager(timeMan), mInputManager(inputMan), mText2D(graphMan),
	mSoundManager(soundMan),
	//mPhysicsManager(mTimeManager, mGraphicsManager),
	mBphysicsManager(mBphysics),
	//mMoveableBody(mInputManager, mPhysicsManager),
	mPlainSprite(plainSprite),
	mSpriteBatch(mTimeManager, mGraphicsManager),
	mAsteroids(mTimeManager, mBphysicsManager, mGraphicsManager, soundMan),
	mBackGround(mGraphicsManager, mTimeManager, plainSprite),
	mSpaceShip(mGraphicsManager, mTimeManager, soundMan, plainSprite, mBphysicsManager),
	mSky(mGraphicsManager, mTimeManager, BACKGROUND_STARS),
	mPlanetEarth(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)EARTH_SCALE, PLANET_EARTH),
	mPlanetMoon(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)MOON_SCALE, PLANET_MOON),
	mPlanetMars(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)MARS_SCALE, PLANET_MARS),
	mPlanetJupiter(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)JUPITER_SCALE, PLANET_JUPITER),
	mPlanetSaturn(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)SATURN_SCALE, PLANET_SATURN),
	mPlanetSun(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)SATURN_SCALE, PLANET_SUN),
	mPlanetMercury(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)MERCURY_SCALE, PLANET_MERCURY),
	mPlanetVenus(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)VENUS_SCALE, PLANET_VENUS),
	mPlanetUranus(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)URANUS_SCALE, PLANET_URANUS),
	mPlanetNeptune(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)NEPTUNE_SCALE, PLANET_NEPTUNE),
	//mLandScape(mTimeManager, mGraphicsManager, mBphysics, (glm::vec3&)LAND_SCALE, HEIGHTMAP_PATH, LAND_TEXTURE),
	mPrimitive(mGraphicsManager, primObj)
	//mStarField(mTimeManager, mGraphicsManager, 800, STARS_TEXTURE)
{
	Log::info("GameScreen constructor");

	//mSky = new SkyBox(graphMan);
}

GameScreen::~GameScreen()
{
	Log::info("GameScreen destructor");
}

void GameScreen::initialize()
{
	// objects init
	mSpaceShip.initialize();
	mAsteroids.initialize();

	mText2D.initialize(28);

	mPlanetEarth.initialize();
	mPlanetEarth.setPosition(EARTH_POS);

	mPlanetMoon.initialize();
	mPlanetMoon.setPosition(MOON_POS);

	mPlanetMars.initialize();
	mPlanetMars.setPosition(MARS_POS);

	mPlanetJupiter.initialize();
	mPlanetJupiter.setPosition(JUPITER_POS);

	mPlanetSaturn.initialize();
	mPlanetSaturn.setPosition(SATURN_POS);
	
	mSpriteBatch.initialize();
	mPlainSprite.setActive(true);

	static BmoveableBody moonBody = mPlanetMoon.getMoveableBody();
	static BmoveableBody earthBody = mPlanetEarth.getMoveableBody();
	static btRigidBody* body = moonBody.getBody();
	earthBody.createCircularHinge(&body);
	moonBody.setVelocity(glm::vec3(0.f, 0.f, 10.f));
	//moonBody.setAngularVelocity(glm::vec3(0.f, 0.1f, 0.f));

	mPlanetSun.initialize();
	mPlanetSun.setPosition(SUN_POS);
	mPlanetSun.setLightPos(SUN_POS);
	mPlanetSun.setLightColor(SUN_COLOR);

	mPlanetMercury.initialize();
	mPlanetMercury.setPosition(MERCURY_POS);

	mPlanetVenus.initialize();
	mPlanetVenus.setPosition(VENUS_POS);

	mPlanetUranus.initialize();
	mPlanetUranus.setPosition(URANUS_POS);

	mPlanetNeptune.initialize();
	mPlanetNeptune.setPosition(NEPTUNE_POS);

	//mLandScape.initialize();
	//mLandScape.setPosition(LAND_POS);
}

status GameScreen::screenLoad()
{
	Log::info("GameScreen load");

	mSky.load();
	//mSky->load();

	mText = "Sample text";

	mColor.r = 0.0;
	mColor.g = 0.0;
	mColor.b = 0.0;
	mColor.a = 0.5;
	
	mTextColor.r = 1.0f;
	mTextColor.g = 1.0f;
	mTextColor.b = 1.0f;
	mTextColor.a = 1.0f;

	mTextPos.x = mGraphicsManager.getRenderWidth(); //500.0f; // -(strlen("Life 10") * (30 / 3.5));
	mTextPos.y = mGraphicsManager.getRenderHeight() - 25.f;
	mTextPos.z = mGraphicsManager.getDepth() / 2;

	mIndexes.push_back(0); mIndexes.push_back(1);
	mIndexes.push_back(2); mIndexes.push_back(2);
	mIndexes.push_back(1); mIndexes.push_back(3);

	for (int j = 0; j < 4; ++j)
	{
		mVertices.push_back(Vertex());
	}

	prepVrtx(&mVertices[0]);

	// Creates and retrieves shader attributes and uniforms.
	mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
	if (mShaderProgram == 0)
		goto ERROR;

	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	uColor = glGetUniformLocation(mShaderProgram, "uColor");

	mProjection = mGraphicsManager.getOrthoMVP();
	
	return STATUS_OK;

ERROR:
	Log::error("MainScreen load failed");

	return STATUS_KO;
}

void GameScreen::screenDraw()
{
	mSky.draw();	
	//mSky->draw();
	
	glUseProgram(mShaderProgram);
	glUniformMatrix4fv(uProjection, 1, GL_FALSE, &mProjection[0][0]);

	glEnable(GL_BLEND);
	// pass color data
	glUniform4fv(uColor, 1, &mColor.r);
	// pass xyz vertex data
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, // Attribute Index
		3,						// Size in bytes (x y and z)
		GL_FLOAT,				// Data type
		GL_FALSE,				// Normalized
		sizeof(Vertex),			// Stride
		&(mVertices[0].x));		// Location

	glDrawElements(GL_TRIANGLES,
		// Number of indexes
		6,
		// Indexes data type
		GL_UNSIGNED_SHORT,
		// First index
		&mIndexes[0]);

	glUseProgram(0);
	glDisableVertexAttribArray(aPosition);
	glDisable(GL_BLEND);

	mText2D.RenderText(mText, mTextPos, 1.f, mTextColor);
}

void GameScreen::screenUpdate()
{
	static float sensitivity = 1000.f;
	//mSky.setPos(glm::vec3(Mouse::getMouseX() * sensitivity, Mouse::getMouseY() * sensitivity, mGraphicsManager.getCamPosition().z * sensitivity));
	//mSky.setPos(glm::vec3(mGraphicsManager.getViewMatrix()[0][0], mGraphicsManager.getViewMatrix()[1][0], mGraphicsManager.getViewMatrix()[2][0]) * 1000.f);

	mSky.update();
	//mSky->update();
	mBackGround.update();
	mSpaceShip.update();
	mAsteroids.update();
	mPlanetEarth.update();
	mPlanetMoon.update();
	mPlanetMars.update();
	mPlanetJupiter.update();
	mPlanetSaturn.update();
	mPlanetSun.update();
	mPlanetMercury.update();
	mPlanetVenus.update();
	mPlanetUranus.update();
	mPlanetNeptune.update();
	//mLandScape.update();

	static float lastTime = 0.0;
	static float currentTime = 0.0;
	// Measure speed
	currentTime += mTimeManager.elapsed();
	nbFrames++;
	if (currentTime - lastTime >= 1.0)
	{
		Log::frames(" %d ", nbFrames); 
		nbFrames = 0;
		lastTime += 1.0;
	}
	
	mCameraPos = mSpaceShip.getThrusterPos();
	mCamFocusPos = mSpaceShip.getPosition();

	// top - down view
	//mCameraPos.y -= 200;
	//mCameraPos.z += 300;

	// 3D view.
	mCameraPos.x = mCamFocusPos.x;
	mCameraPos.y = mCamFocusPos.y - 120;
	mCameraPos.z = mCamFocusPos.z + 30;

	mGraphicsManager.setCamPosition(mCameraPos);
	mGraphicsManager.setCamLookAt(mCamFocusPos);
}

bool GameScreen::screenDrawFinished()
{
	return false;
}

void GameScreen::prepVrtx(Vertex pVertices[4])
{
	// Draws selected frame.
	GLfloat posX1 = 0.0;
	GLfloat posY1 = mGraphicsManager.getRenderHeight() - (mGraphicsManager.getRenderHeight() / 16);
	GLfloat posX2 = mGraphicsManager.getRenderWidth();
	GLfloat posY2 = mGraphicsManager.getRenderHeight();
	GLfloat posZ1 = mGraphicsManager.getDepth() / 2 - 10;
	GLfloat posZ2 = mGraphicsManager.getDepth() / 2 - 10;

	pVertices[0].x = posX1; pVertices[0].y = posY1; pVertices[0].z = posZ1;
	pVertices[1].x = posX1; pVertices[1].y = posY2; pVertices[1].z = posZ2;
	pVertices[2].x = posX2; pVertices[2].y = posY1; pVertices[2].z = posZ1;
	pVertices[3].x = posX2; pVertices[3].y = posY2; pVertices[3].z = posZ2;
}
