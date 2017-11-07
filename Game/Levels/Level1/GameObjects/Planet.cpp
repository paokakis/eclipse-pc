#include "Planet.h"
#include "../Engine/tools/Log.hpp"
#include "../Game/common/MapCoordinates.h"
#include "../Engine/Graphics/Configuration/Folders.h"
#include "../Engine/Graphics/Configuration/Colors.h"

#define ROTATION_SPEED	(0.05f)

int Planet::mId = 300;

Planet::Planet(TimeManager& timeMan, GraphicsManager& graphMan, BphysicsManager& physMan, glm::vec3& scale, planet_type_t type) :
	mTimeManager(timeMan), mGraphicsManager(graphMan), mPlanetType(type), 
	pObj3D(NULL), pPlanetSun(NULL), pPlanetEarth(NULL), mPlanetBody(physMan), mAngle(90.f), mScale(scale)
{
	Log::info("Planet constructor");

	switch (mPlanetType)
	{
	case PLANET_EARTH:
		mPosition = EARTH_POS;
		mScale = EARTH_SCALE;
		//pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, EARTH_PATH, EARTH_TEXTURE, OBJECT3D_NORMALS, mId++, EARTH_NORMAL);
		//mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);

		pPlanetEarth = new PlanetEarth(mGraphicsManager, mTimeManager, mPosition, mScale, EARTH_PATH, EARTH_TEXTURE, OBJECT3D_GENERIC, mId++);
		mPlanetBody.registerMoveableBody(mPosition, pPlanetEarth->getMesh()->at(0).width, pPlanetEarth->getMesh()->at(0).height, pPlanetEarth->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		break;
	case PLANET_MOON:
		mPosition = MOON_POS;
		mScale = MOON_SCALE;
		pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, MOON_PATH, MOON_TEXTURE, OBJECT3D_NORMALS, mId++, MOON_NORMAL);
		mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		//mBody->setDamping(1.f, 0.f);
		//mBody->setSleepingThresholds(1.f, 0.f);
		//mBody = mPlanetBody.registerMoveableBodyMesh(mPosition, pObj3D->getMesh(), mScale, SHAPE_MESH);
		//mPlanetBody.setRotation(glm::vec3(1.f, 0.f, 0.f), mAngle);
		break;
	case PLANET_MARS:
		mPosition = MARS_POS;
		mScale = MARS_SCALE;
		pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, MARS_PATH, MARS_TEXTURE, OBJECT3D_GENERIC, mId++);
		mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		//mBody->setDamping(1.f, 0.f);
		//mBody->setSleepingThresholds(1.f, 0.f);
		//mBody = mPlanetBody.registerMoveableBodyMesh(mPosition, pObj3D->getMesh(), mScale, SHAPE_MESH);
		//mPlanetBody.setRotation(glm::vec3(1.f, 0.f, 0.f), mAngle);
		break;
	case PLANET_JUPITER:
		mPosition = JUPITER_POS;
		mScale = JUPITER_SCALE;
		pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, JUPITER_PATH, JUPITER_TEXTURE, OBJECT3D_GENERIC, mId++);
		mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		//mBody->setDamping(1.f, 0.f);
		//mBody->setSleepingThresholds(1.f, 0.f);
		//mBody = mPlanetBody.registerMoveableBodyMesh(mPosition, pObj3D->getMesh(), mScale, SHAPE_MESH);
		//mPlanetBody.setRotation(glm::vec3(1.f, 0.f, 0.f), mAngle);
		break;
	case PLANET_SATURN:
		mPosition = JUPITER_POS;
		mScale = JUPITER_SCALE;
		pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, SATURN_PATH, SATURN_TEXTURE, OBJECT3D_GENERIC, mId++);
		mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		//mBody->setDamping(1.f, 0.f);
		//mBody->setSleepingThresholds(1.f, 0.f);
		//mBody = mPlanetBody.registerMoveableBodyMesh(mPosition, pObj3D->getMesh(), mScale, SHAPE_MESH);
		//mPlanetBody.setRotation(glm::vec3(1.f, 0.f, 0.f), mAngle);
		break;
	case PLANET_SUN:
		mPosition = SUN_POS;
		mScale = SUN_SCALE;
		pPlanetSun = new PlanetSun(mGraphicsManager, mTimeManager, mPosition, mScale, SUN_PATH, SUN_TEXTURE, OBJECT3D_GENERIC, mId++);
		//pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, SUN_PATH, SUN_TEXTURE, OBJECT3D_CUSTOM_SUN, mId++);
		//pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, SUN_PATH, SUN_TEXTURE, OBJECT3D_GENERIC, mId++);
		//pObj3D->addTextureDiffuse(SUN_TEXTURE2);
		//pObj3D->setLightPos(MARS_POS);
		//mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		//pPlanetSun->setColor(SUN_COLOR);
		//pPlanetSun->setLightPos(SUN_POS);
		mPlanetBody.registerMoveableBody(mPosition, pPlanetSun->getMesh()->at(0).width, pPlanetSun->getMesh()->at(0).height, pPlanetSun->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		//mBody->setDamping(1.f, 0.f);
		//mBody->setSleepingThresholds(1.f, 0.f);
		//mBody = mPlanetBody.registerMoveableBodyMesh(mPosition, pObj3D->getMesh(), mScale, SHAPE_MESH);
		//mPlanetBody.setRotation(glm::vec3(1.f, 0.f, 0.f), mAngle);
		break;
	case PLANET_MERCURY:
		mPosition = MERCURY_POS;
		mScale = MERCURY_SCALE;
		pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, MERCURY_PATH, MERCURY_TEXTURE, OBJECT3D_GENERIC, mId++);
		mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		break;
	case PLANET_VENUS:
		mPosition = VENUS_POS;
		mScale = VENUS_SCALE;
		pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, VENUS_PATH, VENUS_TEXTURE, OBJECT3D_GENERIC, mId++);
		mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		break;
	case PLANET_URANUS:
		mPosition = URANUS_POS;
		mScale = URANUS_SCALE;
		pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, URANUS_PATH, URANUS_TEXTURE, OBJECT3D_GENERIC, mId++);
		mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		break;
	case PLANET_NEPTUNE:
		mPosition = NEPTUNE_POS;
		mScale = NEPTUNE_SCALE;
		pObj3D = new Object3D(mGraphicsManager, mTimeManager, mPosition, mScale, NEPTUNE_PATH, NEPTUNE_TEXTURE, OBJECT3D_GENERIC, mId++);
		mPlanetBody.registerMoveableBody(mPosition, pObj3D->getMesh()->at(0).width, pObj3D->getMesh()->at(0).height, pObj3D->getMesh()->at(0).depth, mScale, SHAPE_SPHERE);
		break;
	}
}

void Planet::initialize()
{
	Log::info("Planet initialize");	

	if (pObj3D != NULL || pPlanetSun != NULL || pPlanetEarth != NULL)
	{
		mPlanetBody.setAngularVelocity(glm::vec3(0, 0, ROTATION_SPEED));
		if (pObj3D != NULL) pObj3D->initialize();
		else if (pPlanetSun != NULL) pPlanetSun->initialize();
		else if (pPlanetEarth != NULL) pPlanetEarth->initialize();
	}

}

status Planet::setPosition(const glm::vec3& pos)
{	
	if (pObj3D != NULL || pPlanetSun != NULL || pPlanetEarth != NULL)
	{
		mPosition = pos;
		mPlanetBody.setPosition(mPosition);
		
		return STATUS_OK;
	}

	return STATUS_KO;
}

status Planet::setRotation(const glm::vec3& rot, const float angle)
{
	if (pObj3D != NULL || pPlanetSun != NULL || pPlanetEarth != NULL)
	{
		mRotation = rot;
		mAngle = angle;
		mPlanetBody.setRotation(mRotation, mAngle);

		return STATUS_OK;
	}
	return STATUS_KO;
}

void Planet::update()
{
	if (pObj3D != NULL)
	{
		pObj3D->setPosition(mPlanetBody.getPosition());
		pObj3D->setRotation(mPlanetBody.getRotation());
		//mPlanetBody.getBody()->activate(true);
	}
	else if (pPlanetSun != NULL)
	{
		pPlanetSun->setPosition(mPlanetBody.getPosition());
		pPlanetSun->setRotation(mPlanetBody.getRotation());
	}
	else if (pPlanetEarth != NULL)
	{
		pPlanetEarth->setPosition(mPlanetBody.getPosition());
		pPlanetEarth->setRotation(mPlanetBody.getRotation());
	}
}