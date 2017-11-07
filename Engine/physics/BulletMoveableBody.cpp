#include "BulletMoveableBody.h"
#include "../Tools/Log.hpp"

BmoveableBody::BmoveableBody(BphysicsManager& pPhysicsManager) : 
	mPhysicsManager(pPhysicsManager), mBody(NULL)
{
	Log::info("Bullet moveableBody constructor");
}

void BmoveableBody::registerMoveableBody(glm::vec3& pLocation, float pSizeX, float pSizeY, float pSizeZ, glm::vec3& scale, shape_type_t type, void* userPtr)
{
	Log::info("Bullet Register BmoveableBody Simple");
	btVector3 loc = btVector3(pLocation.x, pLocation.y, pLocation.z);
	btVector3 scl = btVector3(scale.x, scale.y, scale.z);

	float mass = pSizeX * scale.x + pSizeY * scale.y + pSizeZ * scale.z;

	mBody = mPhysicsManager.loadBody(loc, pSizeX, pSizeY, pSizeZ, scl, mass, userPtr, type);
}

void BmoveableBody::registerMoveableBodyMesh(glm::vec3& pLocation, std::vector<Assimp3D>* mesh, glm::vec3& scale, shape_type_t type, void* userPtr)
{
	Log::info("Bullet Register BmoveableBody Mesh");
	float mass = mesh->at(0).width * scale.x + mesh->at(0).height * scale.y + mesh->at(0).depth * scale.z;

	mBody = mPhysicsManager.loadMeshBody(
		btVector3(pLocation.x, pLocation.y, pLocation.z), mesh, 
		mass, btVector3(scale.x, scale.y, scale.z), userPtr);
}

void BmoveableBody::registerStaticMoveableBody(glm::vec3& pLocation, float pSizeX, float pSizeY, float pSizeZ, glm::vec3& scale, shape_type_t type, void* userPtr)
{
	Log::info("Bullet Register BmoveableBody Simple");
	btVector3 loc = btVector3(pLocation.x, pLocation.y, pLocation.z);
	btVector3 scl = btVector3(scale.x, scale.y, scale.z);

	float mass = 0.f;

	mBody = mPhysicsManager.loadBody(loc, pSizeX, pSizeY, pSizeZ, scl, mass, userPtr, type);
}

void BmoveableBody::registerStaticMoveableBodyMesh(glm::vec3& pLocation, std::vector<Assimp3D>* mesh, glm::vec3& scale, shape_type_t type, void* userPtr)
{
	Log::info("Bullet Register BmoveableBody Mesh");
	float mass = 0.f;

	mBody = mPhysicsManager.loadMeshBody(
		btVector3(pLocation.x, pLocation.y, pLocation.z), mesh,
		mass, btVector3(scale.x, scale.y, scale.z), userPtr);
}

void BmoveableBody::initialize()
{
	Log::info("Bullet MoveableBody initialize");

	//if (mBody != NULL) mBody->setLinearVelocity(btVector3(0.0f, 0.0f, 0.f));
}

void BmoveableBody::update()
{

}

void BmoveableBody::createJoint()
{
	mPhysicsManager.createJoint(&mBody);
}