#pragma once
#include "../Engine/Graphics/Object3D.h"
#include "../Game/Levels/Level1/GameObjects/Sun.h"
#include "../Engine/physics/BulletMoveableBody.h"
#include "../Game/Levels/Level1/GameObjects/Sun.h"
#include "../Game/Levels/Level1/GameObjects/Earth.h"

typedef enum {
	PLANET_EARTH,
	PLANET_MOON,
	PLANET_MARS,
	PLANET_JUPITER,
	PLANET_SATURN,
	PLANET_SUN,
	PLANET_MERCURY,
	PLANET_VENUS,
	PLANET_URANUS,
	PLANET_NEPTUNE
} planet_type_t;

class Planet
{
public:
	Planet(TimeManager& timeMan, GraphicsManager& graphMan, BphysicsManager& physMan, glm::vec3& scale, planet_type_t type);
	~Planet() {}

	void initialize();
	void update();

	status setPosition(const glm::vec3& pos);
	status setRotation(const glm::vec3& rot, const float angle);
	void setLightPos(const glm::vec3& pos) { if(pObj3D) pObj3D->setLightPos(pos); }
	void setLightColor(const glm::vec4& color) { if (pObj3D) pObj3D->setLightColor(color); }

	BmoveableBody& getMoveableBody() { return mPlanetBody; }

private:
	Object3D* pObj3D;
	PlanetSun* pPlanetSun;
	PlanetEarth* pPlanetEarth;
	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	BmoveableBody mPlanetBody;

	glm::vec3 mPosition;
	glm::vec3 mScale;
	glm::vec3 mRotation; float mAngle;
	planet_type_t mPlanetType;
	static int mId;
};