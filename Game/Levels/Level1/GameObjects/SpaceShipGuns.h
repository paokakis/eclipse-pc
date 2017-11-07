#ifndef SPACE_SHIP_GUNS_H
#define SPACE_SHIP_GUNS_H
#include <vector>
#include "../../../Engine/Graphics/PlainSprite.h"
#include "../../../Engine/Physics/BulletMoveableBody.h"
#include "../../../sources\glm\glm.hpp"
#include "../../../Game/common/effects/Explosion.h"
#include "../Engine/Graphics/Object3D.h"

#define MAX_AMMO		15

class SpaceShipGuns {
public:
	SpaceShipGuns(BphysicsManager& physMan, TimeManager& pTimeManager, GraphicsManager& pGraphicsManager, 
		glm::vec3& shipPos, glm::vec3& posLeft, glm::vec3& posRight, glm::vec3& rocketPosLeft, glm::vec3& rocketPosRight);
	~SpaceShipGuns();

	bool initialize();

	void fireGun(glm::vec3& fireDir);
	void fireLeftRocket(glm::vec3& fireDir);
	void fireRightRocket(glm::vec3& fireDir);
	void update();
private:
	TimeManager& mTimeManager; GraphicsManager& mGraphicsManager; BphysicsManager& mPhysicsManager;

	void spawn(BmoveableBody* pBody, glm::vec3& pos);
	void spawnLeftRocket(BmoveableBody* pBody, glm::vec3& pos);
	void spawnRightRocket(BmoveableBody* pBody, glm::vec3& pos);
	void deactivate(BmoveableBody* pBody);
	
	glm::vec3 leftPos[MAX_AMMO], rightPos[MAX_AMMO];
	glm::vec3& mShipPos, mPosLeft, mPosRight;
	glm::vec3 mBulletScale;
	glm::vec3 mFireDirection;

	glm::vec3 rocketLeftPos[MAX_AMMO], rocketRightPos[MAX_AMMO];
	glm::vec3& mRocketPosLeft, mRocketPosRight;
	glm::vec3 mRocketScale;
	glm::vec3 mLeftRocketDirection; glm::vec3 mRightRocketDirection;

	typedef struct ammo {
	public:
		ammo(BmoveableBody* pBody, Object3D* pObj3d, Explosion* pExpl, collisionData_t* col)
		{
			PhysBody = pBody;
			object3D = pObj3d;
			explosion = pExpl;
			colData = col;
		}
		BmoveableBody* PhysBody;
		Object3D* object3D;
		Explosion* explosion;
		collisionData_t* colData;
	} ammoInfo_t;

	std::vector<ammoInfo_t*> mLeftGun;
	std::vector<ammoInfo_t*> mRightGun;
	std::vector<ammoInfo_t*> mLeftRocketGun;
	std::vector<ammoInfo_t*> mRightRocketGun;
	std::vector<ammoInfo_t*>::iterator gunIterator;

	float mMinBound;
	float mUpperBound; float mLowerBound;
	float mLeftBound; float mRightBound;
};

#endif
