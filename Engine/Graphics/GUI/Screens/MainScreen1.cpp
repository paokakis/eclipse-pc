#include "MainScreen1.h"
#include "..\..\Engine\tools\Log.hpp"
#include "../Engine/Graphics/Configuration/Folders.h"
#include "../Game/common/MapCoordinates.h"

static const char* CREDITS_STR =
R"crdts(	CREDITS

Special thanks to:
Opengameart.com
...
)crdts";

#define CUBE_ID			(100)

static glm::vec2 clickPos(0.1);
static glm::vec3 cubeRot(0.1);
static bool firstClick = true;

static const float TOTAL_TIME = 10.f;

MainScreen::MainScreen(GraphicsManager& graphMan, TimeManager& timeMan, InputManager& inpMan, BphysicsManager& mBphysics) :
	mGraphicsManager(graphMan), mTimeManager(timeMan), mInputManager(inpMan), mText(graphMan), mCreditsText(graphMan), 
	mNextScreen(false), mBphysicsManager(mBphysics), mStartMoveableBody(mBphysics), mOptionsMoveableBody(mBphysics), 
	mCreditsMoveableBody(mBphysics), mQuitMoveableBody(mBphysics), mEarthMoveableBody(mBphysics), mTimeout(0), mCubeText(graphMan), 
	mCubeSize(500.f), mMoonMoveableBody(mBphysics)
{
	Log::info("MainScreen constructor");

	float height = mGraphicsManager.getRenderHeight();
	float width = mGraphicsManager.getRenderWidth();
	float dep = mGraphicsManager.getDepth();

	//mSky = new SkyBox(graphMan);
	mSky = new Sky(mGraphicsManager, mTimeManager, BACKGROUND_LINES);

	mCubeStartPos = glm::vec3(width / 2.8f, height / 1.7, dep - 500.f);
	mCubeStartScale = glm::vec3(60, 20, 5);

	mCubeOptionsPos = glm::vec3(mCubeStartPos.x, mCubeStartPos.y - 50, dep - 500.f);
	mCubeOptionsScale = glm::vec3(60, 20, 5);

	mCubeCreditsPos = glm::vec3(mCubeStartPos.x, mCubeOptionsPos.y - 50, dep - 500.f);
	mCubeCreditsScale = glm::vec3(60, 20, 5);

	mCubeQuitPos = glm::vec3(mCubeStartPos.x, mCubeCreditsPos.y - 50, dep - 500.f);
	mCubeQuitScale = glm::vec3(60, 20, 5);

	mEarthPos = glm::vec3(width /1.5, height / 3.f, dep - 1000);
	mEarthScale = glm::vec3(0.5f);

	mMoonPos = glm::vec3(width / 2.f, height / 2.f, dep - 1000);
	mMoonScale = glm::vec3(0.2f);

	mMenuPos = glm::vec3(width / 3.7, height, dep - 300);
	mMenuScale = glm::vec3(0.01);
	
	mStartCube = new Object3D(mGraphicsManager, mTimeManager, mCubeStartPos, mCubeStartScale, std::string(CUBE_OBJ_PATH), std::string(CUBE_TEXTURE_PATH), OBJECT3D_CUBE, CUBE_ID);
	mStartCube->setSilhuetteEnabled(true);
	mStartCube->setColor(glm::vec4(0.2f, 0.5f, 0.3f, 0.8f));
	mOptionsCube = new Object3D(mGraphicsManager, mTimeManager, mCubeOptionsPos, mCubeOptionsScale, std::string(CUBE_OBJ_PATH), std::string(CUBE_TEXTURE_PATH), OBJECT3D_CUBE, CUBE_ID + 1);
	mOptionsCube->setColor(glm::vec4(0.2f, 0.5f, 0.3f, 0.8f));
	mCreditsCube = new Object3D(mGraphicsManager, mTimeManager, mCubeCreditsPos, mCubeCreditsScale, std::string(CUBE_OBJ_PATH), std::string(CUBE_TEXTURE_PATH), OBJECT3D_CUBE, CUBE_ID + 2);
	mCreditsCube->setColor(glm::vec4(0.2f, 0.5f, 0.3f, 0.8f));
	mQuitCube = new Object3D(mGraphicsManager, mTimeManager, mCubeQuitPos, mCubeQuitScale, std::string(CUBE_OBJ_PATH), std::string(CUBE_TEXTURE_PATH), OBJECT3D_CUBE, CUBE_ID + 3);
	mQuitCube->setColor(glm::vec4(0.2f, 0.5f, 0.3f, 0.8f));
	mEarthObj = new Object3D(mGraphicsManager, mTimeManager, mEarthPos, mEarthScale, std::string(EARTH_PATH), std::string(EARTH_TEXTURE), OBJECT3D_GENERIC, CUBE_ID + 4);
	mMoonObj = new Object3D(mGraphicsManager, mTimeManager, mMoonPos, mMoonScale, std::string(MOON_PATH), std::string(MOON_TEXTURE), OBJECT3D_GENERIC, CUBE_ID + 5);
	mMenuPanel = new Object3D(mGraphicsManager, mTimeManager, mMenuPos, mMenuScale, std::string(CUBE_OBJ_PATH), std::string(""), OBJECT3D_CUBE, CUBE_ID + 6);
	
	// keyboard setup
	this->registerKey(GLFW_KEY_UP);
	this->registerKey(GLFW_KEY_DOWN);
	this->registerKey(GLFW_KEY_ENTER);
	mEventManager = EventManager::getInstance();
	mEventManager->registerObject(this);
}

MainScreen::~MainScreen()
{
	Log::info("MainScreen destructor");
}

void MainScreen::initialize()
{
	mInputManager.setMouseShow(false);
	mCubeText.initialize(150);

	mCubeTexVec.push_back(mCubeText.drawOnTexture("Start Game", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mStartCube->setTexture(mCubeTexVec.back());
	mCubeTexVec.push_back(mCubeText.drawOnTexture("Options", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mOptionsCube->setTexture(mCubeTexVec.back());
	mCubeTexVec.push_back(mCubeText.drawOnTexture("Credits", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mCreditsCube->setTexture(mCubeTexVec.back());
	mCubeTexVec.push_back(mCubeText.drawOnTexture("Exit", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mQuitCube->setTexture(mCubeTexVec.back());
	mCubeTexVec.push_back(mCubeText.drawOnTexture("Test", mCubeTextPos, mCubeTextColor, mCubeBackColor, (int)mCubeSize, (int)mCubeSize));
	mMenuPanel->setTexture(mCubeTexVec.back());

	mStartMoveableBody.registerMoveableBodyMesh(mCubeStartPos, mStartCube->getMesh(), mCubeStartScale, SHAPE_MESH);
	mStartMoveableBody.setPosition(mCubeStartPos);
	mStartMoveableBody.setRotation(glm::vec3(0.f, 1.f, 0.f), 180.f);
	mStartMoveableBody.setRotation(glm::vec3(1.f, 0.f, 0.f), 180.f);
	mStartMoveableBody.createJoint();

	mOptionsMoveableBody.registerMoveableBodyMesh(mCubeOptionsPos, mOptionsCube->getMesh(), mCubeOptionsScale, SHAPE_MESH);
	mOptionsMoveableBody.setPosition(mCubeOptionsPos);
	mOptionsMoveableBody.setRotation(glm::vec3(0.f, 1.f, 0.f), 180.f);
	mOptionsMoveableBody.setRotation(glm::vec3(1.f, 0.f, 0.f), 180.f);
	mOptionsMoveableBody.createJoint();

	mCreditsMoveableBody.registerMoveableBodyMesh(mCubeCreditsPos, mCreditsCube->getMesh(), mCubeCreditsScale, SHAPE_MESH);
	mCreditsMoveableBody.setPosition(mCubeCreditsPos);
	mCreditsMoveableBody.setRotation(glm::vec3(0.f, 1.f, 0.f), 180.f);
	mCreditsMoveableBody.setRotation(glm::vec3(1.f, 0.f, 0.f), 180.f);
	mCreditsMoveableBody.createJoint();

	mQuitMoveableBody.registerMoveableBodyMesh(mCubeQuitPos, mQuitCube->getMesh(), mCubeQuitScale, SHAPE_MESH);
	mQuitMoveableBody.setPosition(mCubeQuitPos);
	mQuitMoveableBody.setRotation(glm::vec3(0.f, 1.f, 0.f), 180.f);
	mQuitMoveableBody.setRotation(glm::vec3(1.f, 0.f, 0.f), 180.f);
	mQuitMoveableBody.createJoint();

	mEarthMoveableBody.registerMoveableBodyMesh(mEarthPos, mEarthObj->getMesh(), mEarthScale);
	mEarthMoveableBody.setRotation(glm::vec3(1.f, 0.f, 0.f), 270.f);
	mEarthMoveableBody.setAngularVelocity(glm::vec3(0.f, 0.1f, 0.f));

	mMoonMoveableBody.registerMoveableBodyMesh(mMoonPos, mMoonObj->getMesh(), mMoonScale);
	mMoonMoveableBody.setRotation(glm::vec3(1.f, 0.f, 0.f), 270.f);

	static btRigidBody* body = mMoonMoveableBody.getBody();
	mEarthMoveableBody.createCircularHinge(&body);
	mMoonMoveableBody.setVelocity(glm::vec3(0.f, 0.f, 10.f));

	mEarthObj->setLightPos(mGraphicsManager.getCamPosition());
	mMoonObj->setLightPos(mGraphicsManager.getCamPosition());

	mStartCube->setColor(mCubeSelectedColor);
	mOptionsCube->setColor(mCubeBackColor);
	mCreditsCube->setColor(mCubeBackColor);
	mQuitCube->setColor(mCubeBackColor);

	mMenuPanel->setPosition(mMenuPos);
	mMenuPanel->setRotation(glm::vec4(0.f, 1.f, 0.f, 110.f));
	mMenuPanel->setColor(glm::vec4(0.5, 0.5, 0.5, 1.0));

	// mouse setup
	mMouseEventsLoc = MOUSE_PICKING_NO_DRAG;
	mEventManager->registerObject(&mMouseEventsLoc, this);

	mTextPos = glm::vec3(mGraphicsManager.getRenderWidth() - 50, mGraphicsManager.getRenderHeight() - 50, 500);
	mTextColor.r = 0.7f;
	mTextColor.g = 0.7f;
	mTextColor.b = 0.7f;
	mTextColor.a = 1.0f;

	mText.initialize(30);
	mCreditsText.initialize(20);

	mStartCube->initialize();
	mOptionsCube->initialize();
	mCreditsCube->initialize();
	mQuitCube->initialize();
	mEarthObj->initialize();
	mMoonObj->initialize();
	mMenuPanel->initialize();
	mMenuPanel->setEnabled(false);

	mp_GUI = CedGUI::getInstance();
	mp_GUI->init("GUI");
	mp_GUI->loadScheme("AlfiskoSkin.scheme");
	mp_GUI->loadScheme("OgreTray.scheme");
	mp_GUI->setFont("DejaVuSans-10");
	CEGUI::PushButton* testButton = static_cast<CEGUI::PushButton*> (mp_GUI->createWidget(
		"OgreTray/Button", 
		glm::vec4(mGraphicsManager.getRenderWidth() / 2, mGraphicsManager.getRenderHeight() / 2, 0.1f, 0.05f), 
		glm::vec4(0.f), "TestButton"));
	testButton->setText("Exit");
	// subscribe event handler function.
	testButton->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MainScreen::onTestButtonClickEvent, this));

	CEGUI::ProgressBar* testProgressBar = static_cast<CEGUI::ProgressBar*> (mp_GUI->createWidget(
		"AlfiskoSkin/ProgressBar",
		glm::vec4(mGraphicsManager.getRenderWidth() / 4, mGraphicsManager.getRenderHeight() / 5, 0.4f, 0.1f),
		glm::vec4(0.f), "TestProgressBar")
	);
	testProgressBar->setProgress(.5f);

	CEGUI::Scrollbar* testFrame = static_cast<CEGUI::Scrollbar*> (mp_GUI->createWidget(
		"AlfiskoSkin/HorizontalScrollbar",
		glm::vec4(mGraphicsManager.getRenderWidth() / 6, mGraphicsManager.getRenderHeight() / 6, 0.4f, 0.1f),
		glm::vec4(0.f), "TestFrameWindow"));

	//CEGUI::Combobox* testCombobox = static_cast<CEGUI::Combobox*> (mp_GUI->createWidget(
	//	"AlfiskoSkin/Combobox",
	//	glm::vec4(mGraphicsManager.getRenderWidth() / 4, mGraphicsManager.getRenderHeight() / 4, 0.1f, 0.05f),
	//	glm::vec4(0.f), "TestCombobox"));

	mp_GUI->setMouseCursor("AlfiskoSkin/MouseArrow");
	mp_GUI->showMouseCursor();
}

status MainScreen::screenLoad()
{
	Log::info("MainScreen load");
	mSky->load();

	mCubeTextColor.r = 0.85f;
	mCubeTextColor.g = 0.85f;
	mCubeTextColor.b = 0.85f;
	mCubeTextColor.a = 1.0f;

	mCubeBackColor.r = 0.2f;
	mCubeBackColor.g = 0.4f;
	mCubeBackColor.b = 0.5f;
	mCubeBackColor.a = 0.8f;

	mCubeSelectedColor = glm::vec4(0.7, 0.7, 0.7, 1.0);

	mCubePressedColor = glm::vec4(0.4, 0.4, 0.4, 1.0);

	mCubeTextPos.x = mCubeSize / 2.f;
	mCubeTextPos.y = mCubeSize / 2.f;
	mCubeTextPos.z = 0;

	return STATUS_OK;

__ERROR__:
	Log::error("MainScreen load failed");

	return STATUS_KO;
}

void MainScreen::screenDraw()
{
	mSky->draw();
	mText.RenderText(std::string(GAME_TITLE), mTextPos, 1, mTextColor);
	if (mCreditsEnabled)
	{
		mCreditsText.RenderText(std::string(CREDITS_STR), mCreditsTextPos, 1, mTextColor);
		mCreditsTextPos.y -= (mCreditsTextSpeed * mTimeManager.elapsed());
	}
	mp_GUI->draw();
}

void MainScreen::screenUpdate()
{
	mStartCube->setPosition(mStartMoveableBody.getPosition());
	mStartCube->setRotation(mStartMoveableBody.getRotation());

	mOptionsCube->setPosition(mOptionsMoveableBody.getPosition());
	mOptionsCube->setRotation(mOptionsMoveableBody.getRotation());

	mCreditsCube->setPosition(mCreditsMoveableBody.getPosition());
	mCreditsCube->setRotation(mCreditsMoveableBody.getRotation());

	mQuitCube->setPosition(mQuitMoveableBody.getPosition());
	mQuitCube->setRotation(mQuitMoveableBody.getRotation());

	mEarthObj->setPosition(mEarthMoveableBody.getPosition());
	mEarthObj->setRotation(mEarthMoveableBody.getRotation());

	mMoonObj->setPosition(mMoonMoveableBody.getPosition());
	mMoonObj->setRotation(mMoonMoveableBody.getRotation());

	mSky->update();

	mp_GUI->update();
}

bool MainScreen::screenDrawFinished()
{
	if (mNextScreen)
	{
		mStartMoveableBody.removeBody();
		mOptionsMoveableBody.removeBody();
		mCreditsMoveableBody.removeBody();
		mQuitMoveableBody.removeBody();
		mEarthMoveableBody.removeBody();
		mEventManager->unRegisterObject(&mMouseEventsLoc);
		mEventManager->unRegisterObject(this);

		if (mStartCube)
		{
			delete mStartCube;
			mStartCube = NULL;
		}
		if (mOptionsCube) 
		{
			delete mOptionsCube;
			mOptionsCube = NULL;
		}
		if (mCreditsCube)
		{
			delete mCreditsCube;
			mCreditsCube = NULL;
		}
		if (mQuitCube)
		{
			delete mQuitCube;
			mQuitCube = NULL;
		}
		if (mEarthObj)
		{
			delete mEarthObj;
			mEarthObj = NULL;
		}
		if (mMoonObj)
		{
			delete mMoonObj;
			mMoonObj = NULL;
		}
		if (mSky)
		{
			delete mSky;
			mSky = NULL;
		}
		if (mMenuPanel)
		{
			delete mMenuPanel;
			mMenuPanel = NULL;
		}		
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
	lPos.z = mGraphicsManager.getCamPosition().z - 200;
	mStartCube->setLightPos(lPos);
	mOptionsCube->setLightPos(lPos);
	mCreditsCube->setLightPos(lPos);
	mQuitCube->setLightPos(lPos);
	mMenuPanel->setLightPos(lPos);
}

void MainScreen::onLeftBodyPicked(const glm::vec2&, btRigidBody* body)
{
	if (body == mStartMoveableBody.getBody())
	{
		mCubeStartScale.z -= 1.f;
		mStartCube->setColor(mCubePressedColor);
		mStartCube->setSilhuetteEnabled(true);
		mOptionsCube->setSilhuetteEnabled(false);
		mCreditsCube->setSilhuetteEnabled(false);
		mQuitCube->setSilhuetteEnabled(false);
	}
	else if (body == mOptionsMoveableBody.getBody())
	{
		mOptionsCube->setColor(mCubePressedColor);
		mCubeOptionsScale.z -= 1.f;
		mStartCube->setSilhuetteEnabled(false);
		mOptionsCube->setSilhuetteEnabled(true);
		mCreditsCube->setSilhuetteEnabled(false);
		mQuitCube->setSilhuetteEnabled(false);
	}
	else if (body == mCreditsMoveableBody.getBody())
	{
		mCreditsCube->setColor(mCubePressedColor);
		mCubeCreditsScale.z -= 1.f;
		mStartCube->setSilhuetteEnabled(false);
		mOptionsCube->setSilhuetteEnabled(false);
		mCreditsCube->setSilhuetteEnabled(true);
		mQuitCube->setSilhuetteEnabled(false);
	}
	else if (body == mQuitMoveableBody.getBody())
	{
		mQuitCube->setColor(mCubePressedColor);
		mCubeQuitScale.z -= 1.f;
		mStartCube->setSilhuetteEnabled(false);
		mOptionsCube->setSilhuetteEnabled(false);
		mCreditsCube->setSilhuetteEnabled(false);
		mQuitCube->setSilhuetteEnabled(true);
	}
	else if (body == mEarthMoveableBody.getBody())
	{

	}
	else if (body == mMoonMoveableBody.getBody())
	{

	}
	else
	{
		// nothing
	}
}

void MainScreen::onLeftBodyReleased(const glm::vec2&, btRigidBody* body)
{
	if (body == mStartMoveableBody.getBody())
	{
		mCubeStartScale.z = 5.f;
		mStartCube->setColor(mCubeSelectedColor);
		mNextScreen = true;
	}
	else if (body == mOptionsMoveableBody.getBody())
	{
		mMenuEnabled = !mMenuEnabled;
		mCubeOptionsScale.z = 5.f;
		mOptionsCube->setColor(mCubeSelectedColor);
		mMenuScale = glm::vec3(mGraphicsManager.getRenderWidth(), mGraphicsManager.getRenderHeight(), 10);
		mMenuPanel->setEnabled(mMenuEnabled);		
	}
	else if (body == mCreditsMoveableBody.getBody())
	{
		mCubeCreditsScale.z = 5.f;
		mCreditsCube->setColor(mCubeSelectedColor);
		mMenuScale = glm::vec3(mGraphicsManager.getRenderWidth(), mGraphicsManager.getRenderHeight(), 10);
		mCreditsEnabled = !mCreditsEnabled;
		if (mCreditsEnabled)
		{
			mCreditsTextPos.x = mGraphicsManager.getRenderWidth();
			mCreditsTextPos.y = mGraphicsManager.getRenderHeight() - 50;
			mCreditsTextPos.z = mGraphicsManager.getDepth() - 200;
		}
	}
	else if (body == mQuitMoveableBody.getBody())
	{
		mCubeQuitScale.z = 5.f;		
		mQuitCube->setColor(mCubeSelectedColor);
		mInputManager.setQuit(true);
	}
	else if (body == mEarthMoveableBody.getBody())
	{
		// do nothing
	}
	else if (body == mMoonMoveableBody.getBody())
	{
		// do nothing
	}
	else
	{
		mStartCube->setColor(mCubeBackColor);
		mOptionsCube->setColor(mCubeBackColor);
		mCreditsCube->setColor(mCubeBackColor);
		mQuitCube->setColor(mCubeBackColor);
	}
}

void MainScreen::keyboardUp(int key)
{
	switch (key)
	{
	case GLFW_KEY_ENTER:
		if (mStartCube->getSilhuetteEnabled())
		{
			mCubeStartScale.z = 5.f;
			mStartCube->setColor(mCubeSelectedColor);
			mNextScreen = true;
		}
		else if (mOptionsCube->getSilhuetteEnabled())
		{
			mMenuEnabled = !mMenuEnabled;
			mCubeOptionsScale.z = 5.f;
			mOptionsCube->setColor(mCubeSelectedColor);
			mMenuScale = glm::vec3(mGraphicsManager.getRenderWidth(), mGraphicsManager.getRenderHeight(), 10);
			mMenuPanel->setEnabled(mMenuEnabled);
		}
		else if (mCreditsCube->getSilhuetteEnabled())
		{
			mCubeCreditsScale.z = 5.f;
			mCreditsCube->setColor(mCubeSelectedColor);
			mMenuScale = glm::vec3(mGraphicsManager.getRenderWidth(), mGraphicsManager.getRenderHeight(), 10);
			mCreditsEnabled = !mCreditsEnabled;
			if (mCreditsEnabled)
			{
				mCreditsTextPos.x = mGraphicsManager.getRenderWidth();
				mCreditsTextPos.y = mGraphicsManager.getRenderHeight() - 50;
				mCreditsTextPos.z = mGraphicsManager.getDepth() - 200;
			}
		}
		else if (mQuitCube->getSilhuetteEnabled())
		{
			mCubeQuitScale.z = 5.f;
			mQuitCube->setColor(mCubeSelectedColor);
			mInputManager.setQuit(true);
		}
		break;
	default:
		break;
	}
}

void MainScreen::keyboardPressed(int key)
{
	//Log::info("keyboardDown %d", key);
	static float timeNow;

	timeNow = mTimeManager.elapsedTotal();

	if (mKeyCoolDown < timeNow)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
			if (mStartCube->getSilhuetteEnabled())
			{
				mQuitCube->setColor(mCubeSelectedColor);
				mStartCube->setColor(mCubeBackColor);
				mOptionsCube->setColor(mCubeBackColor);
				mCreditsCube->setColor(mCubeBackColor);
				mStartCube->setSilhuetteEnabled(false);
				mOptionsCube->setSilhuetteEnabled(false);
				mCreditsCube->setSilhuetteEnabled(false);
				mQuitCube->setSilhuetteEnabled(true);
			}
			else if (mOptionsCube->getSilhuetteEnabled())
			{
				mStartCube->setColor(mCubeSelectedColor);
				mOptionsCube->setColor(mCubeBackColor);
				mCreditsCube->setColor(mCubeBackColor);
				mQuitCube->setColor(mCubeBackColor);
				mStartCube->setSilhuetteEnabled(true);
				mOptionsCube->setSilhuetteEnabled(false);
				mCreditsCube->setSilhuetteEnabled(false);
				mQuitCube->setSilhuetteEnabled(false);
			}
			else if (mCreditsCube->getSilhuetteEnabled())
			{
				mStartCube->setColor(mCubeBackColor);
				mOptionsCube->setColor(mCubeSelectedColor);
				mCreditsCube->setColor(mCubeBackColor);
				mQuitCube->setColor(mCubeBackColor);
				mStartCube->setSilhuetteEnabled(false);
				mOptionsCube->setSilhuetteEnabled(true);
				mCreditsCube->setSilhuetteEnabled(false);
				mQuitCube->setSilhuetteEnabled(false);
			}
			else if (mQuitCube->getSilhuetteEnabled())
			{
				mStartCube->setColor(mCubeBackColor);
				mOptionsCube->setColor(mCubeBackColor);
				mCreditsCube->setColor(mCubeSelectedColor);
				mQuitCube->setColor(mCubeBackColor);
				mStartCube->setSilhuetteEnabled(false);
				mOptionsCube->setSilhuetteEnabled(false);
				mCreditsCube->setSilhuetteEnabled(true);
				mQuitCube->setSilhuetteEnabled(false);
			}
			break;
		case GLFW_KEY_DOWN:
			if (mStartCube->getSilhuetteEnabled())
			{
				mStartCube->setColor(mCubeBackColor);
				mOptionsCube->setColor(mCubeSelectedColor);
				mCreditsCube->setColor(mCubeBackColor);
				mQuitCube->setColor(mCubeBackColor);
				mStartCube->setSilhuetteEnabled(false);
				mOptionsCube->setSilhuetteEnabled(true);
				mCreditsCube->setSilhuetteEnabled(false);
				mQuitCube->setSilhuetteEnabled(false);
			}
			else if (mOptionsCube->getSilhuetteEnabled())
			{
				mStartCube->setColor(mCubeBackColor);
				mOptionsCube->setColor(mCubeBackColor);
				mCreditsCube->setColor(mCubeSelectedColor);
				mQuitCube->setColor(mCubeBackColor);
				mStartCube->setSilhuetteEnabled(false);
				mOptionsCube->setSilhuetteEnabled(false);
				mCreditsCube->setSilhuetteEnabled(true);
				mQuitCube->setSilhuetteEnabled(false);
			}
			else if (mCreditsCube->getSilhuetteEnabled())
			{
				mStartCube->setColor(mCubeBackColor);
				mOptionsCube->setColor(mCubeBackColor);
				mCreditsCube->setColor(mCubeBackColor);
				mQuitCube->setColor(mCubeSelectedColor);
				mStartCube->setSilhuetteEnabled(false);
				mOptionsCube->setSilhuetteEnabled(false);
				mCreditsCube->setSilhuetteEnabled(false);
				mQuitCube->setSilhuetteEnabled(true);
			}
			else if (mQuitCube->getSilhuetteEnabled())
			{
				mStartCube->setColor(mCubeSelectedColor);
				mOptionsCube->setColor(mCubeBackColor);
				mCreditsCube->setColor(mCubeBackColor);
				mQuitCube->setColor(mCubeBackColor);
				mStartCube->setSilhuetteEnabled(true);
				mOptionsCube->setSilhuetteEnabled(false);
				mCreditsCube->setSilhuetteEnabled(false);
				mQuitCube->setSilhuetteEnabled(false);
			}
			break;
		}
		mKeyCoolDown = timeNow + mKeyTime;
	}	
}

void MainScreen::onTestButtonClickEvent()
{
	mInputManager.setQuit(true);
}