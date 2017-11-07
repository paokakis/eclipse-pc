#include "SpaceShipGuns.h"
#include "../../../Engine/Tools/Log.hpp"
#include "../Engine/Graphics/Configuration/Folders.h"

static const int	PHYSICS_GROUP = 2;
static const float	GUN_VELOCITY = 10000.0f;
static const float	ROCKET_VELOCITY = 5000.f;

int AMMO_ID = 700;

SpaceShipGuns::SpaceShipGuns(BphysicsManager& physMan, TimeManager& pTimeManager, GraphicsManager& pGraphicsManager, 
	glm::vec3& shipPos, glm::vec3& posLeft, glm::vec3& posRight, glm::vec3& rocketPosLeft, glm::vec3& rocketPosRight) :
	mPhysicsManager(physMan),
	mTimeManager(pTimeManager), mGraphicsManager(pGraphicsManager),
	mMinBound(0.0f), 
	mUpperBound(0.0f), mLowerBound(0.0f),
	mLeftBound(0.0f), mRightBound(0.0f),
	leftPos(), rightPos(), mBulletScale(0.01f), mRocketScale(1.f),
	mShipPos(shipPos), mPosLeft(posLeft), mPosRight(posRight),
	mRocketPosLeft(rocketPosLeft), mRocketPosRight(rocketPosRight)
{
	for (uint16_t i = 0; i < MAX_AMMO; ++i)
	{
		leftPos[i] = shipPos + posLeft;
		leftPos[i].y += 15;
		rightPos[i] = shipPos + posRight;
		rightPos[i].y += 15;

		rocketLeftPos[i] = shipPos + rocketPosLeft;
		rocketLeftPos[i].y += 15;
		rocketRightPos[i] = shipPos + rocketPosRight;
		rocketRightPos[i].y += 15;

		mLeftGun.push_back(
			new ammoInfo_t(
			new BmoveableBody(mPhysicsManager), 
			new Object3D(mGraphicsManager, mTimeManager, leftPos[i], mBulletScale, BULLET_PATH, BULLET_TEXTURE, OBJECT3D_GENERIC, AMMO_ID++), 
			new Explosion(mGraphicsManager, mTimeManager, 30),
			new collisionData_t(false)
			));
		mRightGun.push_back(
			new ammoInfo_t(
			new BmoveableBody(mPhysicsManager), 
			new Object3D(mGraphicsManager, mTimeManager, rightPos[i], mBulletScale, BULLET_PATH, BULLET_TEXTURE, OBJECT3D_GENERIC, AMMO_ID++), 
			new Explosion(mGraphicsManager, mTimeManager, 30), 
			new collisionData_t(false)
			));
		mLeftRocketGun.push_back(
			new ammoInfo_t(
			new BmoveableBody(mPhysicsManager),
			new Object3D(mGraphicsManager, mTimeManager, rocketLeftPos[i], mRocketScale, ROCKET_PATH, ROCKET_TEXTURE, OBJECT3D_GENERIC, AMMO_ID++),
			new Explosion(mGraphicsManager, mTimeManager, 30),
			new collisionData_t(false)
		));
		mRightRocketGun.push_back(
			new ammoInfo_t(
			new BmoveableBody(mPhysicsManager),
			new Object3D(mGraphicsManager, mTimeManager, rocketRightPos[i], mRocketScale, ROCKET_PATH, ROCKET_TEXTURE, OBJECT3D_GENERIC, AMMO_ID++),
			new Explosion(mGraphicsManager, mTimeManager, 30),
			new collisionData_t(false)
		));
	}
	for (gunIterator = mLeftGun.begin(); gunIterator != mLeftGun.end(); ++gunIterator)
	{
		(*gunIterator)->PhysBody->registerMoveableBody(leftPos[0], (*gunIterator)->object3D->getMesh()->at(0).width, (*gunIterator)->object3D->getMesh()->at(0).height, (*gunIterator)->object3D->getMesh()->at(0).depth, mBulletScale, SHAPE_SPHERE);
		(*gunIterator)->PhysBody->setPosition(glm::vec3(-1000000, -1000000, RANDOM(-1000000)));
		(*gunIterator)->PhysBody->setRotation(glm::vec3(1, 1, 1), 90.f);

		(*gunIterator)->PhysBody->setCollisionCb(&(*gunIterator)->colData);
	}

	for (gunIterator = mRightGun.begin(); gunIterator != mRightGun.end(); ++gunIterator)
	{
		(*gunIterator)->PhysBody->registerMoveableBody(rightPos[0], (*gunIterator)->object3D->getMesh()->at(0).width, (*gunIterator)->object3D->getMesh()->at(0).height, (*gunIterator)->object3D->getMesh()->at(0).depth, mBulletScale, SHAPE_SPHERE);
		(*gunIterator)->PhysBody->setPosition(glm::vec3(-1000000, -1000000, RANDOM(-1000000)));
		(*gunIterator)->PhysBody->setRotation(glm::vec3(1, 1, 1), 90.f);

		(*gunIterator)->PhysBody->setCollisionCb(&(*gunIterator)->colData);
	}

	for (gunIterator = mLeftRocketGun.begin(); gunIterator != mLeftRocketGun.end(); ++gunIterator)
	{
		(*gunIterator)->PhysBody->registerMoveableBody(rocketLeftPos[0], (*gunIterator)->object3D->getMesh()->at(0).width, (*gunIterator)->object3D->getMesh()->at(0).height, (*gunIterator)->object3D->getMesh()->at(0).depth, mBulletScale, SHAPE_SPHERE);
		(*gunIterator)->PhysBody->setPosition(glm::vec3(-1000000, -1000000, RANDOM(-1000000)));

		(*gunIterator)->PhysBody->setCollisionCb(&(*gunIterator)->colData);
	}

	for (gunIterator = mRightRocketGun.begin(); gunIterator != mRightRocketGun.end(); ++gunIterator)
	{
		(*gunIterator)->PhysBody->registerMoveableBody(rocketRightPos[0], (*gunIterator)->object3D->getMesh()->at(0).width, (*gunIterator)->object3D->getMesh()->at(0).height, (*gunIterator)->object3D->getMesh()->at(0).depth, mBulletScale, SHAPE_SPHERE);
		(*gunIterator)->PhysBody->setPosition(glm::vec3(-1000000, -1000000, RANDOM(-1000000)));

		(*gunIterator)->PhysBody->setCollisionCb(&(*gunIterator)->colData);
	}
}

bool SpaceShipGuns::initialize()
{
	Log::info("SpaceShip guns init");
	
	for (gunIterator = mLeftGun.begin(); gunIterator != mLeftGun.end(); ++gunIterator)
	{
		(*gunIterator)->object3D->initialize();
	}

	for (gunIterator = mRightGun.begin(); gunIterator != mRightGun.end(); ++gunIterator)
	{
		(*gunIterator)->object3D->initialize();
	}

	for (gunIterator = mLeftRocketGun.begin(); gunIterator != mLeftRocketGun.end(); ++gunIterator)
	{
		(*gunIterator)->object3D->initialize();
	}

	for (gunIterator = mRightRocketGun.begin(); gunIterator != mRightRocketGun.end(); ++gunIterator)
	{
		(*gunIterator)->object3D->initialize();
	}
	
	return true;
}

void SpaceShipGuns::fireGun(glm::vec3& fireDir)
{
	static int leftBodyCount = 0;
	static int rightBodyCount = 0;
	mFireDirection = fireDir;

	leftPos[leftBodyCount] = glm::vec3(mPosLeft + mShipPos);
	spawn(mLeftGun[leftBodyCount]->PhysBody, leftPos[leftBodyCount]);

	rightPos[rightBodyCount] = glm::vec3(mPosRight + mShipPos);
	spawn(mRightGun[rightBodyCount]->PhysBody, rightPos[rightBodyCount]);

	if (leftBodyCount < MAX_AMMO - 1)
	{
		leftBodyCount++;
		rightBodyCount++;
	}
	else
	{
		leftBodyCount = 0;
		rightBodyCount = 0;
	}
}

void SpaceShipGuns::fireLeftRocket(glm::vec3& fireDir)
{
	static int leftBodyCount = 0;
	mLeftRocketDirection = fireDir;

	rocketLeftPos[leftBodyCount] = glm::vec3(mRocketPosLeft + mShipPos);
	spawnLeftRocket(mLeftRocketGun[leftBodyCount]->PhysBody, rocketLeftPos[leftBodyCount]);

	if (leftBodyCount < MAX_AMMO - 1)
	{
		leftBodyCount++;
	}
	else
	{
		leftBodyCount = 0;
	}
}

void SpaceShipGuns::fireRightRocket(glm::vec3& fireDir)
{
	static int rightBodyCount = 0;
	mRightRocketDirection = fireDir;

	rocketRightPos[rightBodyCount] = glm::vec3(mRocketPosRight + mShipPos);
	spawnRightRocket(mRightRocketGun[rightBodyCount]->PhysBody, rocketRightPos[rightBodyCount]);

	if (rightBodyCount < MAX_AMMO - 1)
	{
		rightBodyCount++;
	}
	else
	{
		rightBodyCount = 0;
	}
}

void SpaceShipGuns::update()
{
	glm::vec3 pos;
	glm::vec4 rotation;

	for (gunIterator = mLeftGun.begin(); gunIterator != mLeftGun.end(); ++gunIterator)
	{
		pos = (*gunIterator)->PhysBody->getPosition();
		rotation = (*gunIterator)->PhysBody->getRotation();

		(*gunIterator)->object3D->setPosition(pos);
		(*gunIterator)->object3D->setRotation(rotation);

		if ((*gunIterator)->colData->collision)
		{
			(*gunIterator)->explosion->start(pos, 1.f);
			(*gunIterator)->colData->collision = false;
			deactivate((*gunIterator)->PhysBody);
		}
	}
	for (gunIterator = mRightGun.begin(); gunIterator != mRightGun.end(); ++gunIterator)
	{
		pos = (*gunIterator)->PhysBody->getPosition();
		rotation = (*gunIterator)->PhysBody->getRotation();

		(*gunIterator)->object3D->setPosition(pos);
		(*gunIterator)->object3D->setRotation(rotation);

		if ((*gunIterator)->colData->collision)
		{
			(*gunIterator)->explosion->start(pos, 1.f);
			(*gunIterator)->colData->collision = false;
			deactivate((*gunIterator)->PhysBody);
		}
	}
	for (gunIterator = mLeftRocketGun.begin(); gunIterator != mLeftRocketGun.end(); ++gunIterator)
	{
		pos = (*gunIterator)->PhysBody->getPosition();
		rotation = (*gunIterator)->PhysBody->getRotation();

		(*gunIterator)->object3D->setPosition(pos);
		(*gunIterator)->object3D->setRotation(rotation);

		if ((*gunIterator)->colData->collision)
		{
			(*gunIterator)->explosion->start(pos, 1.f);
			(*gunIterator)->colData->collision = false;
			deactivate((*gunIterator)->PhysBody);
		}
	}
	for (gunIterator = mRightRocketGun.begin(); gunIterator != mRightRocketGun.end(); ++gunIterator)
	{
		pos = (*gunIterator)->PhysBody->getPosition();
		rotation = (*gunIterator)->PhysBody->getRotation();

		(*gunIterator)->object3D->setPosition(pos);
		(*gunIterator)->object3D->setRotation(rotation);

		if ((*gunIterator)->colData->collision)
		{
			(*gunIterator)->explosion->start(pos, 1.f);
			(*gunIterator)->colData->collision = false;
			deactivate((*gunIterator)->PhysBody);
		}
	}
}

SpaceShipGuns::~SpaceShipGuns()
{

}

void SpaceShipGuns::spawn(BmoveableBody* pBody, glm::vec3& pos)
{
	pBody->setPosition(pos);
	pBody->setVelocity((mFireDirection * GUN_VELOCITY)); //glm::vec3(0.f, VELOCITY, 0.f));
	pBody->setRotation(glm::vec3(1, 1, 1), 90.f);
	pBody->setActive(true);
}

void SpaceShipGuns::spawnLeftRocket(BmoveableBody* pBody, glm::vec3& pos)
{
	pBody->setPosition(pos);
	pBody->setVelocity((mLeftRocketDirection * ROCKET_VELOCITY)); //glm::vec3(0.f, VELOCITY, 0.f));
	pBody->setRotation(glm::vec3(0.5f, 0.f, 0.f), 90.f);
	pBody->setActive(true);
}

void SpaceShipGuns::spawnRightRocket(BmoveableBody* pBody, glm::vec3& pos)
{
	pBody->setPosition(pos);
	pBody->setVelocity(glm::abs(mRightRocketDirection * ROCKET_VELOCITY)); //glm::vec3(0.f, VELOCITY, 0.f));
	pBody->setRotation(glm::vec3(0.5f, 0.f, 0.f), 90.f);
	pBody->setActive(true);
}

void SpaceShipGuns::deactivate(BmoveableBody* pBody)
{
	pBody->setPosition(glm::vec3(-1000000, -1000000, RANDOM(-1000000)));
	pBody->setVelocity(glm::vec3(0.f, 0.f, 0.f));
	pBody->setActive(false);
}