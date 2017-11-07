#include "MainScreen.h"
#include "..\..\Engine\tools\Log.hpp"
#include "../Engine/Graphics/Configuration/Folders.h"

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

#define CUBE_ID				(100)

static glm::vec2 clickPos(0.1);
static glm::vec3 cubeRot(0.1);
static bool firstClick = true;

static const float TOTAL_TIME = 10.f;

MainScreen::MainScreen(GraphicsManager& graphMan, TimeManager& timeMan, InputManager& inpMan, BphysicsManager& mBphysics) :
	mGraphicsManager(graphMan), mTimeManager(timeMan), mInputManager(inpMan), mNextScreen(false),
	mBphysicsManager(mBphysics), mStartMoveableBody(mBphysics),
	mSky(graphMan, timeMan, BACKGROUND_LINES), mTimeout(0), mCubeText(graphMan), mCubeSize(500.f), 
	mCubePos(mGraphicsManager.getRenderWidth() / 2, mGraphicsManager.getRenderHeight() / 2, mGraphicsManager.getDepth() - 400.f),
	mCubeScale(50, 50, 50)
{
	Log::info("MainScreen constructor");

	mCube = new Object3D(mGraphicsManager, mTimeManager, mCubePos, mCubeScale, std::string(CUBE_OBJ_PATH), std::string(CUBE_TEXTURE_PATH), OBJECT3D_CUBE, CUBE_ID);
	mCube->setColor(glm::vec4(0.2f, 0.5f, 0.3f, 0.8f));

	mEventManager = EventManager::getInstance();
}

MainScreen::~MainScreen()
{
	Log::info("MainScreen destructor");
}

void MainScreen::initialize()
{
	mInputManager.setMouseShow(true);
	mCubeText.initialize(150);

	mCubeTexVec.push_back(mCubeText.drawOnTexture("Start Game", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mCubeTexVec.push_back(mCubeText.drawOnTexture("Options", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mCubeTexVec.push_back(mCubeText.drawOnTexture("Exit", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mCubeTexVec.push_back(mCubeText.drawOnTexture("Credits", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mCubeTexVec.push_back(mCubeText.drawOnTexture("test 1", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mCubeTexVec.push_back(mCubeText.drawOnTexture("test 2", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));

	mCube->setTexture(mCubeTexVec);
		
	mStartMoveableBody.registerMoveableBodyMesh(mCubePos, mCube->getMesh(), mCubeScale, SHAPE_MESH);
	mStartMoveableBody.setPosition(mCubePos);
	mStartMoveableBody.setRotation(glm::vec3(1.f ,1.f ,1.f), 180.f);
	mStartMoveableBody.createJoint();
	// mouse setup
	mMouseEventsLoc = MOUSE_PICKING;
	mEventManager->registerObject(&mMouseEventsLoc, this);
}

status MainScreen::screenLoad()
{
	Log::info("MainScreen load");

	mSky.load();

	mCubeTextColor.r = 1.0f;
	mCubeTextColor.g = 0.0f;
	mCubeTextColor.b = 0.0f;
	mCubeTextColor.a = 1.0f;
		
	mCubeBackColor.r = 0.2f;
	mCubeBackColor.g = 0.4f;
	mCubeBackColor.b = 1.0f;
	mCubeBackColor.a = 0.5f;

	mCubeTextPos.x = mCubeSize / 2.f;
	mCubeTextPos.y = mCubeSize / 2.f;
	mCubeTextPos.z = 0;

	// Creates and retrieves shader attributes and uniforms.
	mShaderProgram = mGraphicsManager.loadShader(VERTEX_SHADER, FRAGMENT_SHADER);
	if (mShaderProgram == 0)
		goto ERROR;

	aPosition = glGetAttribLocation(mShaderProgram, "aPosition");
	uProjection = glGetUniformLocation(mShaderProgram, "uProjection");
	uColor = glGetUniformLocation(mShaderProgram, "uColor");
	
	return STATUS_OK;

ERROR:
	Log::error("MainScreen load failed");

	return STATUS_KO;
}

void MainScreen::screenDraw()
{
	mSky.draw();
}

void MainScreen::screenUpdate()
{
	mSky.update();

	mCube->setPosition(mStartMoveableBody.getPosition());
	mCube->setRotation(mStartMoveableBody.getRotation());
}

bool MainScreen::screenDrawFinished()
{
	if (mNextScreen)
	{
		mStartMoveableBody.removeBody();
		mEventManager->unRegisterObject(&mMouseEventsLoc);

		delete mCube;
	}
	return mNextScreen;
}

void MainScreen::prepVrtx(Vertex pVertices[4])
{
	// Draws selected frame.
	GLfloat posX1 = 0.0;
	GLfloat posY1 = 0.0;
	GLfloat posX2 = mGraphicsManager.getRenderWidth();
	GLfloat posY2 = mGraphicsManager.getRenderHeight();
	GLfloat posZ1 = 0.1f;//mGraphicsManager.getDepth() / 2 - 10;
	GLfloat posZ2 = 0.1f;//mGraphicsManager.getDepth() / 2 - 10;

	pVertices[0].x = posX1; pVertices[0].y = posY1; pVertices[0].z = posZ1;
	pVertices[1].x = posX2; pVertices[1].y = posY2; pVertices[1].z = posZ2;
	pVertices[2].x = posX1; pVertices[2].y = posY1; pVertices[2].z = posZ1;
	pVertices[3].x = posX2; pVertices[3].y = posY2; pVertices[3].z = posZ2;
}

void MainScreen::onHover(const glm::vec2& pos)
{
	static glm::vec3 lPos;
	lPos.x = pos.x;
	lPos.y = pos.y;
	lPos.z = mCubePos.z + 200;
	mCube->setLightPos(lPos);
}

void MainScreen::onLeftDrag(const glm::vec2& pos)
{

}

void MainScreen::onLeftClick(const glm::vec2& pos)
{

}

void MainScreen::onLeftRelease(const glm::vec2& pos)
{
	mNextScreen = true;
}