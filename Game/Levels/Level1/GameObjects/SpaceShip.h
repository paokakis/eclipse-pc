#ifndef SPACESHIP_H
#define SPACESHIP_H
#include "SpaceShipGuns.h"
#include "../../../Engine/Physics/PhysicsManager.hpp"
#include "../../../Engine/Physics/BulletPhysicsManager.h"
#include "../../../Engine/Physics/BulletMoveableBody.h"
#include "../../../Engine/Sound/AudioManager.h"
#include "../../../Engine/Sound/Sound.hpp"
#include "../../../Engine/Physics/MoveableBody.hpp"
#include "../../../Engine/Graphics/PlainSprite.h"
#include "../../../Engine/Input/InputManager.hpp"
#include "../../../Game/common/effects/Explosion.h"

#include "../Engine/Graphics/Object3D.h"

typedef enum {
	SPACESHIP_HUMAN,
	SPACESHIP_AI,
	SPACESHIP_ENEMY
} SpaceShipType_t;

class SpaceShip : public KeyboardEventObject, MouseEventObject {
public:
	SpaceShip(GraphicsManager&, TimeManager&, AudioManager&, PlainSprite& pPlSprite, BphysicsManager&, SpaceShipType_t spType = SPACESHIP_HUMAN);

	void initialize();
	void update();

	void fire();
	void fireLeftRocket();
	void fireRightRocket();
	bool isDestroyed();

	glm::vec3& getPosition() { return mSpaceShipPos; }
	glm::vec3& getThrusterPos() { return mThrusterAPos; }
	float getSpeed() { return mDirectionY; }

	~SpaceShip();

protected:
	void keyboardDown(int key);
	void keyboardPressed(int key);

	void onHover(const glm::vec2& pos);
	void onLeftClick(const glm::vec2& pos);
private:
	GraphicsManager& mGraphicsManager; 
	TimeManager& mTimeManager;
	AudioManager& mSoundManager;
	PlainSprite& mPlainSprite;
	Object3D* mShipObject;
	Object3D* mThrusterAObj;
	EventManager* pEventManager;
	collisionData_t* data;
	sEventType_t mMouseEventsLoc;
	Explosion mExplosion;

	sAnimOptions_t mainBodyAnimOpts; int32_t mMainAnimId;

	glm::vec3 mSpaceShipPos;
	glm::vec3 mThrusterAPos;
	glm::vec3 mTargetPos;
	glm::vec3 mShipScale;
	glm::vec3 mThrusterAScale;
	glm::vec3 mDirection;

	Sound* mCollisionSound;
	Sound* mLaserSound;
	Sound* mExplosionSound;

	SpaceShipGuns guns;

	BmoveableBody mMoveableBody;
	BmoveableBody mThrusterABody;
	BmoveableBody mMainBody;
	bool mDestroyed; int32_t mLives;
	bool firstMouse = false;
	bool mLeftRocket, mRightRocket;

	float mDirectionX = 0.f, mDirectionY = 0.f, mDirectionZ = 0.f;
	bool mFire;
	
	// spaceship attributes
	const float mMaxSpeed = 150.f;
	const float mMinSpeed = -150.f;
	float mSpaceShipDefence = 10;
};

#endif // SPACESHIP_H
