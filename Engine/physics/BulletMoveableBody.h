#ifndef BULLET_MOVEABLE_H
#define BULLET_MOVEABLE_H

#include "../Input/InputManager.hpp"
#include "BulletPhysicsManager.h"
#include "../Tools/Types.hpp"
#include "../../sources/glm/glm.hpp"

class BmoveableBody {
public:
	BmoveableBody(BphysicsManager& pPhysicsManager);
	void registerMoveableBody(glm::vec3& pLocation, float pSizeX, float pSizeY, float pSizeZ, glm::vec3& scale, shape_type_t type, void* userPtr = NULL);
	void registerMoveableBodyMesh(glm::vec3& pLocation, std::vector<Assimp3D>* mesh, glm::vec3& scale, shape_type_t type = SHAPE_MESH, void* userPtr = NULL);
	void registerStaticMoveableBody(glm::vec3& pLocation, float pSizeX, float pSizeY, float pSizeZ, glm::vec3& scale, shape_type_t type, void* userPtr = NULL);
	void registerStaticMoveableBodyMesh(glm::vec3& pLocation, std::vector<Assimp3D>* mesh, glm::vec3& scale, shape_type_t type = SHAPE_MESH, void* userPtr = NULL);
	
	void createCompoundMeshBody(glm::vec3& pLocation, std::vector<Assimp3D>* mesh, float pDensity, glm::vec3& scale, void* userPtr = nullptr)
	{
		mBody = mPhysicsManager.createCompoundMeshBody(btVector3(pLocation.x, pLocation.y, pLocation.z), mesh, pDensity, btVector3(scale.x, scale.y, scale.z), userPtr);
	}
	bool addCompoundChild(glm::vec3& pLocation, float pSizeX, float pSizeY, float pSizeZ, glm::vec3& scale, shape_type_t type, void* data = nullptr)
	{
		return mPhysicsManager.addCompoundChild(btVector3(pLocation.x, pLocation.y, pLocation.z), mBody, pSizeX, pSizeY, pSizeZ, btVector3(scale.x, scale.y, scale.z), type, data);
	}
	void createCircularHinge(btRigidBody** bodyB)
	{
		mPhysicsManager.createCircularJoint(&mBody, bodyB);
	}
	void createJoint();
	void createSliderJoint(btRigidBody** bodyB, glm::vec3& lowerLimit, glm::vec3& higherLimit) 
	{
		mPhysicsManager.createSliderJoint(&mBody, bodyB, btVector3(lowerLimit.x, lowerLimit.y, lowerLimit.z), 
			btVector3(higherLimit.x, higherLimit.y, higherLimit.z));
	}

	void initialize();
	void update();

	void setSleepingThresholds(float x, float y)
	{
		mBody->setSleepingThresholds(0.f, 0.f);
	}
	void setActive(bool state)
	{
		mPhysicsManager.setActive(&mBody, state);
	}
	bool getActivation()
	{
		return mPhysicsManager.getActivation(&mBody);
	}
	glm::vec3& getPosition()
	{
		return mPhysicsManager.getPosition(&mBody);
	}
	glm::vec3& getCompoundPos(int index)
	{
		return mPhysicsManager.getCompoundChildPos(&mBody, index);
	}
	glm::vec4& getCompoundRotation(int index)
	{
		return mPhysicsManager.getCompoundRotation(&mBody, index);
	}
	void setPosition(glm::vec3& Position)
	{
		mPhysicsManager.setPosition(&mBody, Position);
	}
	glm::vec4& getRotation()
	{
		return mPhysicsManager.getRotation(&mBody);
	}
	void setRotation(float yaw, float pitch, float roll)
	{
		mPhysicsManager.setRotation(&mBody, yaw, pitch, roll);
	}
	void setRotation(glm::vec3& Rotation, float angle)
	{
		mPhysicsManager.setRotation(&mBody, Rotation, angle);
	}
	void setVelocity(glm::vec3& velocity)
	{
		mPhysicsManager.setVelocity(&mBody, velocity);
	}
	glm::vec3& getVelocity()
	{
		return mPhysicsManager.getVelocity(&mBody);
	}
	void setAngularVelocity(glm::vec3& vel)
	{
		mPhysicsManager.setAngularVelocity(&mBody, vel);
	}
	glm::vec3& getAngularVelocity()
	{
		return mPhysicsManager.getAngularVelocity(&mBody);
	}
	void applyImpulse(glm::vec3& impulse, glm::vec3& impPos)
	{
		mPhysicsManager.applyImpulse(&mBody, impulse, impPos);
	}
	void applyTorque(glm::vec3& torque)
	{
		mPhysicsManager.applyTorque(&mBody, torque);
	}
	void applyForce(const glm::vec3& force, const glm::vec3& rel_pos)
	{
		mPhysicsManager.applyForce(&mBody, force, rel_pos);
	}
	void clearForces()
	{
		mPhysicsManager.clearForces(&mBody);
	}
	void removeBody()
	{
		mPhysicsManager.removeBody(&mBody);
	}
	void* getUserPointer()
	{
		return mPhysicsManager.getUserPointer(&mBody);
	}
	void setCollisionCb(collisionData_t** data)
	{
		mPhysicsManager.setCollisionCb(mBody, data);
	}
	btRigidBody* getBody()
	{
		return mBody;
	}
	void applyCentralForce(glm::vec3& relativeForce)
	{
		mPhysicsManager.applyCentralForce(&mBody, relativeForce);
	}
	void applyTorqueImpulse(glm::vec3& torque)
	{
		mPhysicsManager.applyTorqueImpulse(&mBody, torque);
	}
private:
	BphysicsManager& mPhysicsManager;
	int collision;

	btRigidBody* mBody;
};

#endif