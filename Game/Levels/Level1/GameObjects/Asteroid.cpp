#include "Asteroid.hpp"
#include "..\..\Engine\tools\Log.hpp"
#include "..\..\Engine\Graphics\Sprite.hpp"
#include "../Engine/Graphics/Configuration/Folders.h"

static const float BOUNDS_MARGIN = 128 / PHYSICS_SCALE;
static const float MIN_VELOCITY = 700.0f / PHYSICS_SCALE;
static const float VELOCITY_RANGE = 4500.0f / PHYSICS_SCALE;

#define ROTATION_SPEED			(2)

int ASTEROID_ID = (110);

const int PHYSICS_GROUP = 2;
const int LIVES = 3;

Asteroid::Asteroid(TimeManager& pTimeManager, BphysicsManager& pPhysicsManager, GraphicsManager& pGraphicsManager, AudioManager& audMan) :
	mTimeManager(pTimeManager),
	mGraphicsManager(pGraphicsManager),
	mPhysicsManager(pPhysicsManager),
	mSoundManager(audMan),
	mMinBound(0.0f),
	mUpperBound(0.0f), mLowerBound(0.0f),
	mLeftBound(0.0f), mRightBound(0.0f)
{
	Log::info("Asteroid constructor");

	std::string strArr[ASTEROID_COUNT];
	strArr[0] = ASTEROID1_OBJ_PATH;
	strArr[1] = ASTEROID2_OBJ_PATH;
	strArr[2] = ASTEROID2_OBJ_PATH;
	strArr[3] = ASTEROID1_OBJ_PATH;
	strArr[4] = ASTEROID2_OBJ_PATH;
	strArr[5] = ASTEROID1_OBJ_PATH;
	strArr[6] = ASTEROID1_OBJ_PATH;
	strArr[7] = ASTEROID2_OBJ_PATH;
	strArr[8] = ASTEROID1_OBJ_PATH;
	strArr[9] = ASTEROID1_OBJ_PATH;
	strArr[10] = ASTEROID2_OBJ_PATH;
	strArr[11] = ASTEROID2_OBJ_PATH;
	strArr[12] = ASTEROID1_OBJ_PATH;
	strArr[13] = ASTEROID2_OBJ_PATH;
	strArr[14] = ASTEROID1_OBJ_PATH;
	strArr[15] = ASTEROID1_OBJ_PATH;
	strArr[16] = ASTEROID2_OBJ_PATH;
	strArr[17] = ASTEROID1_OBJ_PATH;
	strArr[18] = ASTEROID1_OBJ_PATH;
	strArr[19] = ASTEROID1_OBJ_PATH;
	strArr[20] = ASTEROID2_OBJ_PATH;
	strArr[21] = ASTEROID2_OBJ_PATH;
	strArr[22] = ASTEROID1_OBJ_PATH;
	strArr[23] = ASTEROID2_OBJ_PATH;
	strArr[24] = ASTEROID1_OBJ_PATH;
	strArr[25] = ASTEROID1_OBJ_PATH;
	strArr[26] = ASTEROID2_OBJ_PATH;
	strArr[27] = ASTEROID1_OBJ_PATH;
	strArr[28] = ASTEROID1_OBJ_PATH;
	strArr[29] = ASTEROID1_OBJ_PATH;
	
	// Creates asteroids.
	for (int32_t i = 0; i < ASTEROID_COUNT; ++i)
	{
		objProperties* mProperties = new objProperties;

		mProperties->position = glm::vec3(RANDOM(1000000.f));
		mProperties->scale = glm::vec3(1);
		mProperties->lives = LIVES;		
		mProperties->pObject3D = new Object3D(*GraphicsManager::getInstance(), mTimeManager, mProperties->position, mProperties->scale, strArr[i], ASTEROID_TEXTURE_PATH, OBJECT3D_GENERIC, ASTEROID_ID++);
		mProperties->pMoveableBody = new BmoveableBody(mPhysicsManager);
		mProperties->pMoveableBody->registerMoveableBodyMesh(mProperties->position, mProperties->pObject3D->getMesh(), mProperties->scale, SHAPE_MESH);
		//mProperties->pMoveableBody->registerMoveableBody(mProperties->position, mProperties->pObject3D->getMesh()->at(0).width, mProperties->pObject3D->getMesh()->at(0).height, mProperties->pObject3D->getMesh()->at(0).depth, mProperties->scale, SHAPE_SPHERE);
		//mProperties->pMoveableBody->getBody()->setAngularFactor(1.f);
		//mProperties->pMoveableBody->getBody()->setSleepingThresholds(0.f, 0.f);
		mProperties->pExplosion = new Explosion(mGraphicsManager, mTimeManager, mProperties->pObject3D->getMesh()->at(0).width);
		mProperties->pColData = new collisionData_t(false);
		mProperties->pMoveableBody->setCollisionCb(&mProperties->pColData);
		
		vProperties.push_back(mProperties);
	}
	for (int32_t i = 0; i < ASTEROID_COUNT; ++i)
	{
		objProperties* mProperties = new objProperties;

		mProperties->position = glm::vec3(RANDOM(1000000.f));
		mProperties->scale = glm::vec3(15);
		mProperties->lives = LIVES;
		mProperties->pObject3D = new Object3D(*GraphicsManager::getInstance(), mTimeManager, mProperties->position, mProperties->scale, ASTEROID3_OBJ_PATH, ASTEROID_TEXTURE_PATH3, OBJECT3D_NORMALS, ASTEROID_ID++, ASTEROID_NORMAL_PATH3);
		mProperties->pMoveableBody = new BmoveableBody(mPhysicsManager);
		mProperties->pMoveableBody->registerMoveableBodyMesh(mProperties->position, mProperties->pObject3D->getMesh(), mProperties->scale, SHAPE_MESH);
		//mProperties->pMoveableBody->registerMoveableBody(mProperties->position, mProperties->pObject3D->getMesh()->at(0).width, mProperties->pObject3D->getMesh()->at(0).height, mProperties->pObject3D->getMesh()->at(0).depth, mProperties->scale, SHAPE_SPHERE);
		//mProperties->pMoveableBody->getBody()->setAngularFactor(1.f);
		//mProperties->pMoveableBody->getBody()->setSleepingThresholds(0.f, 0.f);
		mProperties->pExplosion = new Explosion(mGraphicsManager, mTimeManager, mProperties->pObject3D->getMesh()->at(0).width);
		mProperties->pColData = new collisionData_t(false);
		mProperties->pMoveableBody->setCollisionCb(&mProperties->pColData);
		
		vProperties.push_back(mProperties);
	}
	for (int32_t i = 0; i < ASTEROID_COUNT; ++i)
	{
		objProperties* mProperties = new objProperties;

		mProperties->position = glm::vec3(RANDOM(1000000.f));
		mProperties->scale = glm::vec3(8);
		mProperties->lives = LIVES;
		mProperties->pObject3D = new Object3D(*GraphicsManager::getInstance(), mTimeManager, mProperties->position, mProperties->scale, ASTEROID4_OBJ_PATH, ASTEROID_TEXTURE_PATH4, OBJECT3D_NORMALS, ASTEROID_ID++, ASTEROID_NORMAL_PATH4);
		mProperties->pMoveableBody = new BmoveableBody(mPhysicsManager);
		mProperties->pMoveableBody->registerMoveableBodyMesh(mProperties->position, mProperties->pObject3D->getMesh(), mProperties->scale, SHAPE_MESH);
		//mProperties->pMoveableBody->registerMoveableBody(mProperties->position, mProperties->pObject3D->getMesh()->at(0).width, mProperties->pObject3D->getMesh()->at(0).height, mProperties->pObject3D->getMesh()->at(0).depth, mProperties->scale, SHAPE_SPHERE);
		//mProperties->pMoveableBody->getBody()->setAngularFactor(1.f);
		//mProperties->pMoveableBody->getBody()->setSleepingThresholds(0.f, 0.f);
		mProperties->pExplosion = new Explosion(mGraphicsManager, mTimeManager, mProperties->pObject3D->getMesh()->at(0).width);
		mProperties->pColData = new collisionData_t(false);
		mProperties->pMoveableBody->setCollisionCb(&mProperties->pColData);

		vProperties.push_back(mProperties);
	}
}

Asteroid::~Asteroid()
{
	std::vector<objProperties*>::iterator bodyIt;
	for (bodyIt = vProperties.begin(); bodyIt < vProperties.end(); ++bodyIt)
	{
		delete (*bodyIt)->pExplosion;
		delete (*bodyIt)->pMoveableBody;
		delete (*bodyIt)->pColData;
		delete (*bodyIt)->pObject3D;
		delete (*bodyIt);
	}
	vProperties.clear();
}

void Asteroid::initialize() 
{
	Log::info("Asteroid initialize");

	// load sounds
	mExplosionSound = mSoundManager.registerSound(ASTEROID_EXPL_PATH);

	mUpperBound = 2000.f; //mGraphicsManager.getRenderHeight() * *mGraphicsManager.getMVPMatrix();
	mLowerBound = -mUpperBound; //0.1f * *mGraphicsManager.getProjectionMatrix() - 300.f;
	mRightBound = 3000.f; //mGraphicsManager.getRenderWidth() * *mGraphicsManager.getMVPMatrix() * 2;
	mLeftBound = -mRightBound; //0.1f * *mGraphicsManager.getProjectionMatrix() - 200.f;
	mZUpperBound = 4000.f; //mGraphicsManager.getDepth() * *mGraphicsManager.getMVPMatrix() * 2;
	mZLowerBound = -mZUpperBound; //0.1f * *mGraphicsManager.getProjectionMatrix() - 10.f;

	std::vector<objProperties*>::iterator bodyIt;
	for (bodyIt = vProperties.begin(); bodyIt < vProperties.end(); ++bodyIt)
	{
		spawn((*bodyIt));
		(*bodyIt)->pObject3D->initialize();
	}
}

void Asteroid::update() 
{
	std::vector<objProperties*>::iterator bodyIt;
	static float angle = 0;
	for (bodyIt = vProperties.begin(); bodyIt < vProperties.end(); ++bodyIt)
	{
		glm::vec3 pos = (*bodyIt)->pMoveableBody->getPosition();
		glm::vec4 rotation = (*bodyIt)->pMoveableBody->getRotation();

		(*bodyIt)->pObject3D->setPosition(pos);
		(*bodyIt)->pObject3D->setRotation(rotation);
		
		if ((*bodyIt)->lives > 0)
		{
			if ((*bodyIt)->pColData->collision)
			{
				//mSoundManager.playSound(mCollisionSound);
				--(*bodyIt)->lives;
				if ((*bodyIt)->lives < 0)
				{
					glm::vec3 position = (*bodyIt)->pMoveableBody->getPosition();
					(*bodyIt)->pExplosion->start(position, 1.0);
					mSoundManager.playSound(mExplosionSound, position);
				}
			}
		}
		// Destroyed.
		else
		{
			//mSoundManager.playSound(mExplosionSound, (*bodyIt)->pMoveableBody->getPosition());
			//spawn((*bodyIt));
		}
		
	}
}

void Asteroid::spawn(objProperties* prop)
{
	prop->position.x = RANDOM(mRightBound);
	prop->position.y = (mUpperBound);
	prop->position.z = RANDOM(mZUpperBound);

	prop->pMoveableBody->setPosition(prop->position);
	prop->pMoveableBody->setVelocity(glm::vec3(0.f, -(RANDOM(VELOCITY_RANGE) + MIN_VELOCITY), 0.f));
	prop->pMoveableBody->setAngularVelocity(glm::vec3(RANDOM(100) / 100, RANDOM(100) / 100, RANDOM(100) / 100));
	prop->lives = LIVES;
}