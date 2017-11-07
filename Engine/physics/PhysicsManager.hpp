#ifndef PHYSICSMANAGER_HPP
#define PHYSICSMANAGER_HPP
#include "../Graphics/GraphicsManager.hpp"
#include "../../sources/Box2D/Box2D.h"
#include "../TimeManager.hpp"
#include "../Tools/Types.hpp"
#include <vector>
#include "../../sources/glm/glm.hpp"

#define PHYSICS_SCALE 32.0f

struct PhysicsCollision {
	bool collide;
	PhysicsCollision() : collide(false) {}
};

class PhysicsManager : private b2ContactListener {
public:
	PhysicsManager(TimeManager& pTimeManager, GraphicsManager& pGraphicsManager);
	~PhysicsManager();
	b2Body* loadBody(glm::vec3& pLocation, uint16 pCategory, uint16 pMask, int32_t pSizeX, int32_t pSizeY, float pRestitution, int group, float);
	b2Body* loadBulletBody(glm::vec3& pLocation, uint16 pCategory, uint16 pMask, int32_t pSizeX, int32_t pSizeY, float pRestitution, int group, float);

	b2MouseJoint* loadTarget(b2Body* pBodyObj);
	void start();
	void update();
private:
	TimeManager& mTimeManager;
	GraphicsManager& mGraphicsManager;

	void BeginContact(b2Contact* pContact);
	b2World mWorld;
	std::vector<b2Body*> mBodies;
	std::vector<glm::vec3*> mLocations;
	b2Body* mBoundsBodyObj;
};

#endif