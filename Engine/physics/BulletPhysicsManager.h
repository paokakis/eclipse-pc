#ifndef BULLET_PHYSICS_MANAGER_H
#define BULLET_PHYSICS_MANAGER_H
#include "..\tools\Types.hpp"
#include <btBulletDynamicsCommon.h>
#include "glm.hpp"
#include "../Graphics/GraphicsManager.hpp"
#include "../TimeManager.hpp"
#include "BulletDebugDraw.h"
#include "../OS/Scheduler.h"

#define PHYSICS_SCALE 32.0f

typedef volatile struct col_data
{
public:
	col_data(bool col)
	{
		collision = col;
	}
	volatile bool collision = false;
} collisionData_t;

typedef enum {
	SHAPE_SPHERE,
	SHAPE_BOX,
	SHAPE_CYLINDER,
	SHAPE_CAPSULE,
	SHAPE_CONE,
	SHAPE_MULTISPHERE,
	SHAPE_MESH
} shape_type_t;

class BphysicsManager : public Task {
	friend class BmoveableBody;
public:
	BphysicsManager(GraphicsManager& pGraphicsManager, TimeManager& pTimeManager);
	~BphysicsManager();

	static BphysicsManager* getInstance() { return instance; }
	void addConstraint(btTypedConstraint*, bool);
	void removeConstraint(btTypedConstraint*);

	void start();

	bool rayHasHit(glm::vec3& rayFromWorld, glm::vec3& rayToWorld, btVector3* hitPointWorld, btRigidBody** body)
	{
		mMutex->lock();
		static bool ret;
		rayFrom.setX(rayFromWorld.x);
		rayFrom.setY(rayFromWorld.y);
		rayFrom.setZ(rayFromWorld.z);

		rayTo.setX(rayToWorld.x);
		rayTo.setY(rayToWorld.y);
		rayTo.setZ(rayToWorld.z);

		btCollisionWorld::ClosestRayResultCallback rayCallback(rayFrom, rayTo);
		mpDynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

		if (rayCallback.m_collisionObject)
		{
			*body = (btRigidBody*)btRigidBody::upcast(rayCallback.m_collisionObject);
			*hitPointWorld = rayCallback.m_hitPointWorld;
		}

		ret = rayCallback.hasHit();
		mMutex->unlock();

		return ret;
	}

protected:
	void taskRun();

private:
	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	static BphysicsManager* instance;

	void setScreenBoundBox();

	btBroadphaseInterface* mpBroadphase;
	btDefaultCollisionConfiguration* mpCollisionConfiguration;
	btCollisionDispatcher* mpDispatcher;
	btConstraintSolver* mpSolver;
	btDiscreteDynamicsWorld* mpDynamicsWorld;
	btIDebugDraw* mDebugDrawer;

	std::vector<btRigidBody*> vRigidBodies;
	std::vector<btCollisionShape*> vCollisionShapes;
	std::vector<btMotionState*> vMotionStates;
	std::vector<btStridingMeshInterface*> vMeshes;
	std::vector<btPolyhedralConvexAabbCachingShape*> vConvexMeshes;
	std::vector<btTypedConstraint*> vConstrains;

	//typedef struct {
	//	btRigidBody* rigidBody;
	//	btRigidBody* collisionShape;
	//	btMotionState* motionState;
	//	btVector3* position;
	//	btStridingMeshInterface* mesh;
	//} BulletBody_t;

	//std::vector<BulletBody_t*> bulletBodies;

	volatile shape_type_t mBodyType;

	Scheduler* pSched;

	static btVector3 rayFrom;
	static btVector3 rayTo;

private:

	btRigidBody* loadBody(btVector3& pLocation, float pSizeX, float pSizeY, float pSizeZ, btVector3&, float pDensity, void* userPtr, shape_type_t type);
	btRigidBody* loadMeshBody(btVector3& pLocation, std::vector<Assimp3D>* mesh, float pDensity, btVector3& scale, void* userPtr);
	//btRigidBody* loadCompoundBody(btVector3& pLocation, float density, std::vector<btRigidBody*> bodies);

	bool addCompoundChild(btVector3& pLocation, btRigidBody* mainBody, float sizeX, float sizeY, float sizeZ, btVector3& scale, shape_type_t type, void* data);
	btRigidBody* createCompoundMeshBody(btVector3& pLocation, std::vector<Assimp3D>* mesh, float pDensity, btVector3& scale, void* userPtr = NULL);

	bool isCompound(btRigidBody* body)
	{
		static bool comp = false;

		comp = body->getCollisionShape()->isCompound();

		return comp;
	}

	void removeBody(btRigidBody** body);

	void createJoint(btRigidBody** body);
	void createCircularJoint(btRigidBody** bodyA, btRigidBody** bodyB);
	void createSliderJoint(btRigidBody** bodyA, btRigidBody** bodyB, btVector3& lowerSliderLimit, btVector3& hiSliderLimit);

	bool getActivation(btRigidBody** body)
	{
		bool state;
		mMutex->lock();
		state = (*body)->getActivationState();
		mMutex->unlock();

		return state;
	}
	void setActive(btRigidBody** body, bool state)
	{
		mMutex->lock();
		(*body)->activate(state);
		mMutex->unlock();
	}
	glm::vec3& getPosition(btRigidBody** body)
	{
		mMutex->lock();
		static btVector3 pos;
		static glm::vec3 retPos;
		pos = (*body)->getCenterOfMassTransform().getOrigin();
		retPos = glm::vec3(pos.getX(), pos.getY(), pos.getZ());
		mMutex->unlock();
		return (retPos);
	}
	glm::vec4& getRotation(btRigidBody** body)
	{
		mMutex->lock();
		static glm::vec4 Rotation;
		btQuaternion q = (*body)->getCenterOfMassTransform().getRotation();

		Rotation.x = q.x();
		Rotation.y = q.y();
		Rotation.z = q.z();
		Rotation.w = q.getAngle();
		mMutex->unlock();

		return Rotation;
	}
	glm::vec3& getCompoundChildPos(btRigidBody** body, int index)
	{
		mMutex->lock();

		static glm::vec3 retPos(0);
		if ((*body)->getCollisionShape()->isCompound())
		{
			static btVector3 pos;

			btCompoundShape* compShape = reinterpret_cast<btCompoundShape*>((*body)->getCollisionShape());

			pos = (compShape->getChildTransform(index) * (*body)->getCenterOfMassTransform()).getOrigin();
			retPos = glm::vec3(pos.getX(), pos.getY(), pos.getZ());
					
			mMutex->unlock();

			return retPos;
		}
		else
		{
			mMutex->unlock();

			return retPos;
		}
	}
	glm::vec4& getCompoundRotation(btRigidBody** body, int index)
	{
		mMutex->lock();
		static glm::vec4 Rotation;

		if ((*body)->getCollisionShape()->isCompound())
		{

			btCompoundShape* compShape = reinterpret_cast<btCompoundShape*>((*body)->getCollisionShape());

			btQuaternion q = compShape->getChildTransform(index).getRotation();

			Rotation.x = q.x();
			Rotation.y = q.y();
			Rotation.z = q.z();
			Rotation.w = q.getAngle();
		}

		mMutex->unlock();

		return Rotation;

	}
	void setPosition(btRigidBody** body, glm::vec3& Position)
	{
		mMutex->lock();
		btTransform transform = (*body)->getCenterOfMassTransform();
		transform.setOrigin(btVector3(Position.x, Position.y, Position.z));
		(*body)->setCenterOfMassTransform(transform);
		mMutex->unlock();
	}
	void setRotation(btRigidBody** body, float yaw, float pitch, float roll)
	{
		mMutex->lock();
		btTransform transform = (*body)->getCenterOfMassTransform();
		transform.setRotation(btQuaternion(glm::radians(yaw), glm::radians(pitch), glm::radians(roll)));
		(*body)->setCenterOfMassTransform(transform);
		mMutex->unlock();
	}
	void setRotation(btRigidBody** body, glm::vec3& Rotation, float angle)
	{
		mMutex->lock();
		btTransform transform = (*body)->getCenterOfMassTransform();
		transform.setRotation(btQuaternion(btVector3(Rotation.x, Rotation.y, Rotation.z), glm::radians(angle)));
		(*body)->setCenterOfMassTransform(transform);
		mMutex->unlock();
	}
	void setVelocity(btRigidBody** body, glm::vec3& velocity)
	{
		mMutex->lock();
		btTransform transform = (*body)->getCenterOfMassTransform();
		(*body)->setLinearVelocity(btVector3(velocity.x, velocity.y, velocity.z));
		(*body)->setCenterOfMassTransform(transform);
		mMutex->unlock();
	}
	glm::vec3& getVelocity(btRigidBody** body)
	{
		mMutex->lock();
		static glm::vec3 retVel;
		retVel = glm::vec3((*body)->getLinearVelocity().getX(), (*body)->getLinearVelocity().getY(), (*body)->getLinearVelocity().getZ());
		mMutex->unlock();
		return retVel;
	}
	void setAngularVelocity(btRigidBody** body, glm::vec3& vel)
	{
		mMutex->lock();
		btTransform transform = (*body)->getCenterOfMassTransform();
		(*body)->setAngularVelocity(btVector3(vel.x, vel.y, vel.z));
		(*body)->setCenterOfMassTransform(transform);
		mMutex->unlock();
	}
	glm::vec3& getAngularVelocity(btRigidBody** body)
	{
		mMutex->lock();
		static glm::vec3 retVel;
		static btVector3 bAng;
		bAng = (*body)->getAngularVelocity();

		retVel.x = bAng.x();
		retVel.y = bAng.y();
		retVel.z = bAng.z();
		mMutex->unlock();

		return retVel;
	}
	void applyImpulse(btRigidBody** body, glm::vec3& impulse, glm::vec3& impPos)
	{
		mMutex->lock();
		btTransform transform = (*body)->getCenterOfMassTransform();
		(*body)->applyImpulse(btVector3(impulse.x, impulse.y, impulse.z), (*body)->getCenterOfMassPosition());//btVector3(impPos.x, impPos.y, impPos.z));
		(*body)->setCenterOfMassTransform(transform);
		mMutex->unlock();
	}
	void applyTorque(btRigidBody** body, glm::vec3& torque)
	{
		mMutex->lock();
		btTransform transform = (*body)->getCenterOfMassTransform();
		(*body)->applyTorque(btVector3(torque.x, torque.y, torque.z));
		(*body)->setCenterOfMassTransform(transform);
		mMutex->unlock();
	}
	void applyForce(btRigidBody** body, const glm::vec3& force, const glm::vec3& rel_pos)
	{
		mMutex->lock();
		btTransform transform = (*body)->getCenterOfMassTransform();
		(*body)->applyForce(btVector3(force.x, force.y, force.z), btVector3(rel_pos.x, rel_pos.y, rel_pos.z));
		(*body)->setCenterOfMassTransform(transform);
		mMutex->unlock();
	}
	void clearForces(btRigidBody** body)
	{
		mMutex->lock();
		btTransform transform = (*body)->getCenterOfMassTransform();
		(*body)->clearForces();
		(*body)->setCenterOfMassTransform(transform);
		mMutex->unlock();
	}
	void* getUserPointer(btRigidBody** body)
	{
		mMutex->lock();
		void* retVal = (*body)->getUserPointer();
		mMutex->unlock();

		return retVal;
	}
	void setCollisionCb(btRigidBody* body, collisionData_t** data)
	{
		mMutex->lock();
		collData.push_back(std::make_pair(body, *data));
		mMutex->unlock();
	}
	void applyCentralForce(btRigidBody** body, glm::vec3& _relativeForce)
	{
		mMutex->lock();
		static btVector3 relativeForce;

		relativeForce.setX(_relativeForce.x);
		relativeForce.setY(_relativeForce.y);
		relativeForce.setZ(_relativeForce.z);
		btMatrix3x3& boxRot = (*body)->getWorldTransform().getBasis();
		btVector3 correctedForce = boxRot * relativeForce;
		(*body)->applyCentralForce(correctedForce);
		mMutex->unlock();
	}
	void applyTorqueImpulse(btRigidBody** body, glm::vec3& torque)
	{
		mMutex->lock();
		static btVector3 trq;
		trq.setX(torque.x);
		trq.setY(torque.y);
		trq.setZ(torque.z);
		(*body)->applyTorqueImpulse(trq);
		mMutex->unlock();
	}
	
	std::vector<std::pair<btRigidBody*, collisionData_t*>> collData;
};

#endif BULLET_PHYSICS_MANAGER_H