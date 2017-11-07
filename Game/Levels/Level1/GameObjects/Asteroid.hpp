#ifndef ASTEROID_HPP
#define ASTEROID_HPP
#include "..\..\Engine\Sound\AudioManager.h"
#include "..\..\Engine\Sound\Sound.hpp"
#include "../../../Engine/Graphics/GraphicsManager.hpp"

#include "../../../Engine/physics/BulletPhysicsManager.h"
#include "../../../Engine/physics/BulletMoveableBody.h"
#include "../Engine/Graphics/Object3D.h"

#include "../../Engine/Graphics/SpriteBatch.h"
#include "../../../Engine/TimeManager.hpp"
#include "../../../Engine/Tools/Types.hpp"
#include "../../../Game/common/effects/Explosion.h"
#include <vector>

static const int32_t ASTEROID_COUNT = 30;

class Asteroid {
public:
	Asteroid(TimeManager& pTimeManager, BphysicsManager& pPhysicsManager, GraphicsManager& pGraphicsManager, AudioManager& audMan);
	~Asteroid();
	void initialize();
	void update();
private:
	TimeManager& mTimeManager;
	GraphicsManager& mGraphicsManager;
	BphysicsManager& mPhysicsManager;
	AudioManager& mSoundManager;

	struct objProperties {
		BmoveableBody* pMoveableBody;
		Object3D* pObject3D;
		Explosion* pExplosion;
		glm::vec3 position;
		glm::vec3 scale;
		int lives;
		collisionData_t* pColData;
	};

	std::vector<objProperties*> vProperties;

	void spawn(objProperties* prop);

	Sound* mExplosionSound;

	float mMinBound;
	float mUpperBound; float mLowerBound; float mZLowerBound;
	float mLeftBound; float mRightBound; float mZUpperBound;
};

#endif