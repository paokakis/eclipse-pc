#include "SpaceShip.h"
#include <string>
#include "../../../sources\glm\glm.hpp"
#include "../../../Engine/Tools/Log.hpp"
#include "../../../Engine/Tools/Helper.h"
#include "../Engine/Graphics/Configuration/Folders.h"

#define MOUSE_SENSITIVITY	(0.5f)

#define DEFAULT_LIVES		(10)
#define SHIP_W				(170)

static glm::vec3 leftGunPos = glm::vec3(-5, 50, -10);
static glm::vec3 rightGunPos = glm::vec3(5, 50, -10);
static glm::vec3 leftRocketPos = glm::vec3(-25, 50, -10);
static glm::vec3 rightRocketPos = glm::vec3(25, 50, -10);

static const float	REPETITION_FIRE = 0.2; // in seconds
static const int	PHYSICS_GROUP = 1;
static const float	MOVE_SPEED = 1000.0f / PHYSICS_SCALE;

#define SHIP_ID				(101)

SpaceShip::SpaceShip(GraphicsManager& graphicsManager, TimeManager& timeManager,
	AudioManager& soundMan, PlainSprite& pPlSprite, BphysicsManager& pPhysMan, SpaceShipType_t spType) :
	mGraphicsManager(graphicsManager), mTimeManager(timeManager), mSoundManager(soundMan),
	mExplosion(graphicsManager, timeManager, SHIP_W),
	mSpaceShipPos(glm::vec3(10000, 10000, 4500.f)),
	mThrusterAPos(glm::vec3(mSpaceShipPos.x, mSpaceShipPos.y - 150.f, mSpaceShipPos.z)),
	guns(pPhysMan, timeManager, graphicsManager, mSpaceShipPos, leftGunPos, rightGunPos, leftRocketPos, rightRocketPos),
	mCollisionSound(NULL), mFire(false), mLeftRocket(false), mRightRocket(false), mDestroyed(false), mLives(0), mShipScale(0.1), mThrusterAScale(glm::vec3(10, 3, 3)),
	mPlainSprite(pPlSprite), mMoveableBody(pPhysMan), mThrusterABody(pPhysMan), mMainBody(pPhysMan)
{
	Log::info("SpaceShip constructor ");		

	mShipObject = new Object3D(graphicsManager, timeManager, mSpaceShipPos, mShipScale, SHIP_OBJ_PATH, SHIP_TEXTURE_PATH, OBJECT3D_GENERIC, SHIP_ID, SHIP_NORMAL_PATH, true);
	mThrusterAObj = new Object3D(graphicsManager, timeManager, mThrusterAPos, mThrusterAScale, THRUSTER_A_OBJ_PATH, CUBE_TEXTURE_PATH, OBJECT3D_CUBE, SHIP_ID + 1);

	// Physics settings
	mDestroyed = false;
	mLives = DEFAULT_LIVES;

	mMoveableBody.registerMoveableBodyMesh(mSpaceShipPos, mShipObject->getMesh(), mShipScale, SHAPE_MESH);
	mMoveableBody.getBody()->setDamping(0.2f, 0.2f);
	mThrusterABody.registerMoveableBodyMesh(mThrusterAPos, mThrusterAObj->getMesh(), mThrusterAScale, SHAPE_MESH);
	mThrusterABody.getBody()->setDamping(0.2f, 0.2f);

	data = new collisionData_t(false);
	mMoveableBody.setCollisionCb(&data);
	
	mMoveableBody.setSleepingThresholds(0.f, 0.f);
	mThrusterABody.setSleepingThresholds(0.f, 0.f);

	mMoveableBody.setRotation(glm::vec3(0, 0, 1), 180.f);
	mThrusterABody.setRotation(glm::vec3(0, 0, 1), 180.f);
	
	static btRigidBody* thrBody = mThrusterABody.getBody();
	mMoveableBody.createSliderJoint(&(thrBody), glm::vec3(0, 150, 0), glm::vec3(0, 150, 0));
}

void SpaceShip::initialize()
{
	Log::info("SpaceShip initialize");

	// Load sounds
	mCollisionSound = mSoundManager.registerSound(SPACESHIP_COLLISION);
	mLaserSound = mSoundManager.registerSound(SPACESHIP_LASER);
	mExplosionSound = mSoundManager.registerSound(SPACESHIP_EXPLOSION);
	
	// Keyboard setup
	this->registerKey(GLFW_KEY_A);
	this->registerKey(GLFW_KEY_D);
	this->registerKey(GLFW_KEY_W);
	this->registerKey(GLFW_KEY_S);
	this->registerKey(GLFW_KEY_SPACE);
	this->registerKey(GLFW_KEY_LEFT_ALT);
	this->registerKey(GLFW_KEY_LEFT_CONTROL);
	pEventManager = EventManager::getInstance();
	pEventManager->registerObject(this);

	mShipObject->setBillBoardLevel(mLives * mSpaceShipDefence);

	// Mouse setup
	mMouseEventsLoc = MOUSE_FREEMOVE;
	pEventManager->registerObject(&mMouseEventsLoc, this);

	// set guns positions.
	guns.initialize();
	mShipObject->initialize();
}

void SpaceShip::update()
{
	mSpaceShipPos = mMoveableBody.getPosition();
	mThrusterAPos = mThrusterABody.getPosition();

	mThrusterAObj->setRotation(mThrusterABody.getRotation());
	mThrusterAObj->setPosition(mThrusterAPos);

	mShipObject->setRotation(mMoveableBody.getRotation());

	mDirection = (mSpaceShipPos - mThrusterAPos) * mDirectionY;
	mMoveableBody.setVelocity(mDirection);
	//mMoveableBody.setAngularVelocity(glm::vec3(0.f, mDirectionX, 0.f));
	
	if (!mDestroyed)
	{
		guns.update();
		fire();
		fireLeftRocket();
		fireRightRocket();
		mFire = false;
		mLeftRocket = false;
		mRightRocket = false;
	}
	
	if (mLives >= 0)
	{
		if (data->collision)
		{
			mSoundManager.playSound(mCollisionSound, mSpaceShipPos);
			--mLives;
			mShipObject->setBillBoardLevel(mLives * mSpaceShipDefence);
			if (mLives < 0)
			{
				Log::info("SpaceShip has been destroyed");

				mExplosion.start(mSpaceShipPos, 1.5);
			}
			data->collision = false;
		}
	}
	// Destroyed.
	else
	{
		if (mExplosion.finishedAnimating())
		{
			mDestroyed = true;
		}
		else
		{
			static bool expl = true;
			if (expl)
			{
				mSoundManager.playSound(mExplosionSound, mSpaceShipPos);
				expl = false;
			}
		}
	}
}

void SpaceShip::fire()
{
	static float timeNow;
	static float coolDownTime;
	static glm::vec3 bulletDir;

	timeNow = mTimeManager.elapsedTotal();

	if (mFire && coolDownTime < timeNow)
	{
		mSoundManager.playSound(mLaserSound, mSpaceShipPos);
		bulletDir = glm::vec3(mSpaceShipPos.x - mThrusterAPos.x, abs(mSpaceShipPos.y - mThrusterAPos.y), mSpaceShipPos.z - mThrusterAPos.z);
		bulletDir = glm::normalize(bulletDir);
		guns.fireGun(bulletDir);
		coolDownTime = timeNow + REPETITION_FIRE;
	}
}

void SpaceShip::fireLeftRocket()
{
	static float timeNow;
	static float coolDownTime;
	static glm::vec3 bulletDir;

	timeNow = mTimeManager.elapsedTotal();

	if (mLeftRocket && coolDownTime < timeNow)
	{
		mSoundManager.playSound(mLaserSound, mSpaceShipPos);
		bulletDir = glm::vec3(mSpaceShipPos.x - mThrusterAPos.x, abs(mSpaceShipPos.y - mThrusterAPos.y), mSpaceShipPos.z - mThrusterAPos.z);
		bulletDir = glm::normalize(bulletDir);
		guns.fireLeftRocket(bulletDir);
		coolDownTime = timeNow + REPETITION_FIRE;
	}
}

void SpaceShip::fireRightRocket()
{
	static float timeNow;
	static float coolDownTime;
	static glm::vec3 bulletDir;

	timeNow = mTimeManager.elapsedTotal();

	if (mRightRocket && coolDownTime < timeNow)
	{
		mSoundManager.playSound(mLaserSound, mSpaceShipPos);
		bulletDir = glm::vec3(mSpaceShipPos.x - mThrusterAPos.x, abs(mSpaceShipPos.y - mThrusterAPos.y), mSpaceShipPos.z - mThrusterAPos.z);
		bulletDir = glm::normalize(bulletDir);
		guns.fireRightRocket(bulletDir);
		coolDownTime = timeNow + REPETITION_FIRE;
	}
}

SpaceShip::~SpaceShip()
{
	Log::info("SpaceShip Destructor");

	pEventManager->unRegisterObject(&mMouseEventsLoc);
	pEventManager->unRegisterObject(this);
}

bool SpaceShip::isDestroyed()
{
	return mDestroyed;
}

void SpaceShip::keyboardPressed(int key)
{
	if (key == GLFW_KEY_A)
	{
		mDirectionX -= 0.05f;
	}
	if (key == GLFW_KEY_D)
	{
		mDirectionX += 0.05f;
	}
	if (key == GLFW_KEY_W)
	{
		if (mDirectionY < mMaxSpeed)
			mDirectionY += 0.05f;
	}
	if (key == GLFW_KEY_S)
	{
		if (mDirectionY > mMinSpeed)
			mDirectionY -= 0.05f;
	}
}

void SpaceShip::keyboardDown(int key)
{
	switch (key)
	{
	case GLFW_KEY_SPACE:
		mFire = true;
		break;
	case GLFW_KEY_LEFT_CONTROL:
		mLeftRocket = true;
		break;
	case GLFW_KEY_LEFT_ALT:
		mRightRocket = true;
		break;
	default:
		break;
	}
}

void SpaceShip::onHover(const glm::vec2& pos)
{
	static bool firstClick = true;
	static glm::vec2 clickPos;
	static glm::vec3 rot;

	if (firstClick)
	{
		clickPos.x = pos.x - 0.0001f;
		clickPos.y = pos.y - 0.0001f;
		firstClick = false;
	}

	float width = mGraphicsManager.getRenderWidth();
	float height = mGraphicsManager.getRenderHeight();

	float rotX = ((pos.x - clickPos.x) / width);
	float rotY = ((pos.y - clickPos.y) / height);

	rot = glm::vec3(-rotY, rotX, 0.f);

	mMoveableBody.setAngularVelocity(rot);
}

void SpaceShip::onLeftClick(const glm::vec2& pos)
{
	mFire = true;
}