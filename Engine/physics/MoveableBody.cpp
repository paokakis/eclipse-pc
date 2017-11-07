#include "../Tools/Log.hpp"
#include "MoveableBody.hpp"

static const float MOVE_SPEED = 100.0f / PHYSICS_SCALE;

MoveableBody::MoveableBody(InputManager& pInputManager, PhysicsManager& pPhysicsManager) :
	mPhysicsManager(pPhysicsManager),
	mInputManager(pInputManager),
	mBody(NULL), mTarget(NULL)
{
	Log::info("MoveableBody constructor");
}

b2Body* MoveableBody::registerMoveableBody(glm::vec3& pLocation, int32_t pSizeX, int32_t pSizeY, int group, bool* action)
{
	mBody = mPhysicsManager.loadBody(pLocation, 0x2, 0x1, pSizeX, pSizeY, 0.0f, group, float(pSizeX * pSizeY));
	mTarget = mPhysicsManager.loadTarget(mBody);
	//mInputManager.setFireButton(action);

	return mBody;
}

void MoveableBody::initialize() 
{
	Log::info("MoveableBody initialize");

	if (mBody)	mBody->SetLinearVelocity(b2Vec2(0.0f, 0.0f));
}

void MoveableBody::update() 
{
	if (mBody)
	{
		//b2Vec2 target = mBody->GetPosition() + b2Vec2(mInputManager.getDirectionX() * MOVE_SPEED, mInputManager.getDirectionY() * MOVE_SPEED);
		//mTarget->SetTarget(target);
	}
}
