#include "BulletPhysicsManager.h"
#include "..\tools\Log.hpp"

//#define BULLET_DEBUG

BphysicsManager* BphysicsManager::instance;
btVector3 BphysicsManager::rayFrom;
btVector3 BphysicsManager::rayTo;

static void findAndRemoveBody(std::vector<btRigidBody*> bodies, btRigidBody* bodyToBeRemoved, btDiscreteDynamicsWorld* pDynamicsWorld)
{
	std::vector<btRigidBody*>::iterator iter;

	for (iter = bodies.begin(); iter != bodies.end(); ++iter)
	{
		if ((*iter) == bodyToBeRemoved)
		{
			pDynamicsWorld->removeRigidBody((*iter));
			delete *iter;
			bodies.erase(iter);

			return;
		}
	}
}

BphysicsManager::BphysicsManager(GraphicsManager& pGraphicsManager, TimeManager& pTimeManager) : 
	mGraphicsManager(pGraphicsManager), mTimeManager(pTimeManager)
{
	Log::info("Bullet physics constructor");
	// Build the broadphase
	mpBroadphase = new btDbvtBroadphase();
	// Set up the collision configuration and dispatcher
	mpCollisionConfiguration = new btDefaultCollisionConfiguration();
	mpDispatcher = new btCollisionDispatcher(mpCollisionConfiguration);
	// The actual physics solver
	mpSolver = new btSequentialImpulseConstraintSolver;
	// The world.
	mpDynamicsWorld = new btDiscreteDynamicsWorld(mpDispatcher, mpBroadphase, mpSolver, mpCollisionConfiguration);

	mpDynamicsWorld->setGravity(btVector3(0, 0, 0));

	instance = this;

	pSched = Scheduler::getInstance();
	pSched->registerTask(this, 100);
}

void BphysicsManager::start()
{
	Log::info("Bullet physics start");

#ifdef BULLET_DEBUG
	mDebugDrawer = new BulletDebugDraw(mGraphicsManager);
	mpDynamicsWorld->setDebugDrawer(mDebugDrawer);
#endif

	if (mpDynamicsWorld->getDebugDrawer())
		mpDynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_MAX_DEBUG_DRAW_MODE);

	//setScreenBoundBox();
	pSched->startTask(this);
	this->setDelayuSec(3);
}

void BphysicsManager::taskRun()
{
	mpDynamicsWorld->stepSimulation(mTimeManager.elapsed(), 1, 1. / 60);
	if (mpDynamicsWorld->getDebugDrawer()) mpDynamicsWorld->debugDrawWorld();

	// collision detection
	int numManifolds = mpDynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; ++i)
	{
		btPersistentManifold* contactManifold = mpDynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* obA = contactManifold->getBody0(); // our object
		//const btCollisionObject* obB = contactManifold->getBody1(); // enemy object

		int numContacts = contactManifold->getNumContacts();
		for (int j = 0; j < numContacts; ++j)
		{
			btManifoldPoint& pt = contactManifold->getContactPoint(j);
			if (pt.getDistance() < 0.f)
			{
				for (int k = 0; k < collData.size(); ++k)
				{
					if (collData[k].first && collData.size() > 0 && collData[k].second && collData[k].first->getBroadphaseHandle() == obA->getBroadphaseHandle())
						collData[k].second->collision = true;
				}
			}
		}
	}
}

BphysicsManager::~BphysicsManager()
{
	Log::info("Bullet physics destructor");

	pSched->stopTask(this);
	// Clean up

	// delete constrains
	std::vector<btTypedConstraint*>::iterator consIter;
	for (consIter = vConstrains.begin(); consIter != vConstrains.end(); ++consIter)
	{
		mpDynamicsWorld->removeConstraint(*consIter);
		delete *consIter;
	}
	vConstrains.clear();
	// delete collision shapes
	std::vector<btCollisionShape*>::iterator cIter;
	for (cIter = vCollisionShapes.begin(); cIter != vCollisionShapes.end(); ++cIter)
	{
		delete *cIter;
	}
	vCollisionShapes.clear();
	// Delete motion states
	std::vector<btMotionState*>::iterator mIter;
	for (mIter = vMotionStates.begin(); mIter != vMotionStates.end(); ++mIter)
	{
		delete *mIter;
	}
	vMotionStates.clear();
	// delete rigid bodies
	std::vector<btRigidBody*>::iterator rIter;
	for (rIter = vRigidBodies.begin(); rIter != vRigidBodies.end(); ++rIter)
	{
		mpDynamicsWorld->removeRigidBody(*rIter);
		delete *rIter;
	}
	// delete meshes
	std::vector<btStridingMeshInterface*>::iterator meIter;
	for (meIter = vMeshes.begin(); meIter != vMeshes.end(); ++meIter)
	{
		delete *meIter;
	}
	vMeshes.clear();
	//// delete convex meshes
	//std::vector<btPolyhedralConvexAabbCachingShape*>::iterator conIter;
	//for (conIter = vConvexMeshes.begin(); conIter != vConvexMeshes.end(); ++conIter)
	//{
	//	delete *conIter;
	//}
	vConvexMeshes.clear();

	delete mpDynamicsWorld;
	delete mpSolver;
	delete mpDispatcher;
	delete mpCollisionConfiguration;
	delete mpBroadphase;
}

btRigidBody* BphysicsManager::loadBody(btVector3& pLocation, float pSizeX, float pSizeY, float pSizeZ, btVector3& scale , float pDensity, void* userPtr, shape_type_t type)
{
	mMutex->lock();

	btCollisionShape* shape;

	switch (type) {
	case SHAPE_BOX:
		shape = new btBoxShape(btVector3(pSizeX, pSizeY, pSizeZ));
		break;
	case SHAPE_SPHERE:
		shape = new btSphereShape(pSizeX / 2);
		break;
	case SHAPE_CAPSULE:
		shape = new btCapsuleShape(pSizeY, pSizeY / 4);
		break;
	case SHAPE_CYLINDER:
		shape = new btCylinderShape(btVector3(pSizeX / 2, pSizeY / 2, pSizeZ / 2));
		break;
	default:
		shape = new btSphereShape(pSizeX / 2);
		break;
	}

	shape->setMargin(0.001);
	shape->setLocalScaling(scale);
	
	btTransform& startTransform = btTransform();
	startTransform.setIdentity();
	startTransform.setOrigin(pLocation);	//put it to x,y,z coordinates	

	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool isDynamic = (pDensity != 0.f);

	btVector3 localInertia(0, 0, 0);
	if (isDynamic)
		shape->calculateLocalInertia(pDensity, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	vMotionStates.push_back(myMotionState);

	btRigidBody::btRigidBodyConstructionInfo cInfo(pDensity, myMotionState, shape, localInertia);

	btRigidBody* body = new btRigidBody(cInfo);
	body->setWorldTransform(startTransform);

#else
	btRigidBody* body = new btRigidBody(mass, 0, shape, localInertia);
	body->setWorldTransform(startTransform);
#endif//

	if (userPtr) body->setUserPointer(userPtr);

	vCollisionShapes.push_back(shape);
	vRigidBodies.push_back(body);

	body->setAngularFactor(1.f);
	body->setLinearVelocity(btVector3(0, 0, 0));
	body->setAngularVelocity(btVector3(0, 0, 0));
	body->setFriction(1.0);
	body->activate(true);
	body->setSleepingThresholds(0.f, 0.f);
	if (userPtr) body->setUserPointer(userPtr);

	mpDynamicsWorld->addRigidBody(body);

	mMutex->unlock();

	return body;
}

bool BphysicsManager::addCompoundChild(btVector3& pLocation, btRigidBody* mainBody, 
	float sizeX, float sizeY, float sizeZ, btVector3& scale, shape_type_t type, void* data)
{
	mMutex->lock();

	btCollisionShape* shape;
	bool ret = false;
	std::vector<Assimp3D>* mesh;
	int* indices;
	btScalar* vertices;

	switch (type) {
	case SHAPE_BOX:
		shape = new btBoxShape(btVector3(sizeX, sizeY, sizeZ));
		break;
	case SHAPE_SPHERE:
		shape = new btSphereShape(sizeX / 2);
		break;
	case SHAPE_CAPSULE:
		shape = new btCapsuleShape(sizeY, sizeY / 4);
		break;
	case SHAPE_CYLINDER:
		shape = new btCylinderShape(btVector3(sizeX / 2, sizeY / 2, sizeZ / 2));
		break;
	case SHAPE_MESH:
		mesh = reinterpret_cast<std::vector<Assimp3D>*>(data);
		indices = (int*)&mesh->at(0).indices[0];
		vertices = &mesh->at(0).vertices[0].x;

		shape = new btConvexHullShape((const btScalar*)(&mesh->at(0).vertices[0].x), mesh->at(0).vertices.size(), sizeof(btScalar) * 3);
		shape->setMargin(0.001);
		shape->setLocalScaling(scale);
		//shape->optimizeConvexHull();
		break;
	default:
		shape = new btSphereShape(sizeX / 2);
		break;
	}

	if (isCompound(mainBody))
	{
		btCompoundShape* compShape = reinterpret_cast<btCompoundShape*>(mainBody->getCollisionShape());

		btTransform& transform = btTransform();
		transform.setIdentity();
		transform.setOrigin(pLocation);

		compShape->addChildShape(transform, shape);

		//float masses = 1000.f;
		//compShape->calculatePrincipalAxisTransform(&masses, (btTransform&)mainBody->getCenterOfMassTransform(), mainBody->getLocalInertia());

		ret = true;
	}
	
	mMutex->unlock();

	return ret;
}

void BphysicsManager::createJoint(btRigidBody** body)
{
	mMutex->lock();
	//		btRigidBody* body1 = 0;//createRigidBody( mass,trans,shape);
	//		btRigidBody* body1 = createRigidBody( 0.0,trans,0);
	//body1->setActivationState(DISABLE_DEACTIVATION);
	//body1->setDamping(0.3,0.3);

	(*body)->setDamping(0.3, 0.3);
	(*body)->setActivationState(DISABLE_DEACTIVATION);
	btVector3 pivotInA(2, -2, -2);
	btVector3 axisInA(0, 0, 1);

	//	btVector3 pivotInB = body1 ? body1->getCenterOfMassTransform().inverse()(body0->getCenterOfMassTransform()(pivotInA)) : pivotInA;
	//		btVector3 axisInB = body1?
	//			(body1->getCenterOfMassTransform().getBasis().inverse()*(body1->getCenterOfMassTransform().getBasis() * axisInA)) :
	(*body)->getCenterOfMassTransform().getBasis() * axisInA;

#define P2P
#ifdef P2P
	btTypedConstraint* p2p = new btPoint2PointConstraint(**body, pivotInA);
	//btTypedConstraint* p2p = new btPoint2PointConstraint(*body0,*body1,pivotInA,pivotInB);
	//btTypedConstraint* hinge = new btHingeConstraint(*body0,*body1,pivotInA,pivotInB,axisInA,axisInB);
	mpDynamicsWorld->addConstraint(p2p);
	vConstrains.push_back(p2p);
	p2p->setDbgDrawSize(btScalar(5.f));
#else
	btHingeConstraint* hinge = new btHingeConstraint(*body, pivotInA, axisInA);

	//use zero targetVelocity and a small maxMotorImpulse to simulate joint friction
	//float	targetVelocity = 0.f;
	//float	maxMotorImpulse = 0.01;
	float	targetVelocity = 1.f;
	float	maxMotorImpulse = 1.0f;
	hinge->enableAngularMotor(true, targetVelocity, maxMotorImpulse);
	m_dynamicsWorld->addConstraint(hinge);
	vConstrains.push_back(hinge);
	hinge->setDbgDrawSize(btScalar(5.f));
#endif //P2P

	mMutex->unlock();
}

void BphysicsManager::createCircularJoint(btRigidBody** bodyA, btRigidBody** bodyB)
{
	mMutex->lock();

	(*bodyB)->setActivationState(DISABLE_DEACTIVATION);
	//btVector3 pivotPos = (*bodyA)->getCenterOfMassPosition();
	const btVector3 btPivotA((*bodyA)->getCenterOfMassPosition());
	btVector3 btAxisA(0.0f, 0.0f, 1.0f);

	btHingeConstraint* pHinge = new btHingeConstraint((**bodyB), btPivotA, btAxisA);
	mpDynamicsWorld->addConstraint(pHinge);
	vConstrains.push_back(pHinge);
	pHinge->setDbgDrawSize(btScalar(5.f));

	mMutex->unlock();
}

void BphysicsManager::createSliderJoint(btRigidBody** bodyA, btRigidBody** bodyB, btVector3& lowerSliderLimit, btVector3& hiSliderLimit)
{
	mMutex->lock();

	//btVector3 lowerSliderLimit = btVector3(0, 40, 0);
	//btVector3 hiSliderLimit = btVector3(0, 40, 0);
	btVector3 sliderAxis(0, 1, 0);
	btScalar angle = 0.f;//SIMD_RADS_PER_DEG * 10.f;
	btMatrix3x3 sliderOrientation(btQuaternion(sliderAxis, angle));

	btRigidBody* d6body0 = *bodyB;
	d6body0->setActivationState(DISABLE_DEACTIVATION);
	btRigidBody* fixedBody1 = *bodyA;

	btTransform frameInA, frameInB;
	frameInA = btTransform::getIdentity();
	frameInB = btTransform::getIdentity();
	frameInA.setOrigin(btVector3(0., 5., 0.));
	frameInB.setOrigin(btVector3(0., 5., 0.));

	//		bool useLinearReferenceFrameA = false;//use fixed frame B for linear llimits
	bool useLinearReferenceFrameA = true;//use fixed frame A for linear llimits
	btGeneric6DofConstraint* spSlider6Dof = new btGeneric6DofConstraint(*fixedBody1, *d6body0, frameInA, frameInB, useLinearReferenceFrameA);
	spSlider6Dof->setLinearLowerLimit(lowerSliderLimit);
	spSlider6Dof->setLinearUpperLimit(hiSliderLimit);

	//range should be small, otherwise singularities will 'explode' the constraint
	//		spSlider6Dof->setAngularLowerLimit(btVector3(-1.5,0,0));
	//		spSlider6Dof->setAngularUpperLimit(btVector3(1.5,0,0));
	//		spSlider6Dof->setAngularLowerLimit(btVector3(0,0,0));
	//		spSlider6Dof->setAngularUpperLimit(btVector3(0,0,0));
	spSlider6Dof->setAngularLowerLimit(btVector3(0, 0, 0));
	spSlider6Dof->setAngularUpperLimit(btVector3(0, 0, 0));


	mpDynamicsWorld->addConstraint(spSlider6Dof, true);
	vConstrains.push_back(spSlider6Dof);
	spSlider6Dof->setDbgDrawSize(btScalar(5.f));

	mMutex->unlock();
}

void BphysicsManager::removeBody(btRigidBody** body)
{
	mMutex->lock();
	mpDynamicsWorld->removeRigidBody(*body);
	mMutex->unlock();
}

btRigidBody* BphysicsManager::loadMeshBody(btVector3& pLocation, std::vector<Assimp3D>* mesh, float pDensity, btVector3& scale, void* userPtr)
{
	mMutex->lock();
	// pre-filled, just here to show you the declaration
	if (mesh->size() == 1)
	{
		btRigidBody* physicsRigidBody;
		int* indices = (int*)&mesh->at(0).indices[0];
		btScalar* vertices = &mesh->at(0).vertices[0].x;

		//btStridingMeshInterface* physicsMesh = new btTriangleIndexVertexArray(mesh->at(i).indices.size() / 3, indices, sizeof(int) * 3, mesh->at(i).vertices.size(), vertices, sizeof(btScalar) * 3);
		//
		//btCollisionShape* physicsShape = new btBvhTriangleMeshShape(physicsMesh, true, true);
		//physicsShape->setMargin(0.001); 
		//physicsShape->setLocalScaling(scale);
		btConvexHullShape* physicsShape = new btConvexHullShape((const btScalar*)(&mesh->at(0).vertices[0].x), mesh->at(0).vertices.size(), sizeof(btScalar) * 3);
		physicsShape->setMargin(0.001);
		physicsShape->setLocalScaling(scale);
		physicsShape->optimizeConvexHull();
		//physicsShape->initializePolyhedralFeatures();

		btVector3 inertia(0.f, 0.f, 0.f);
		physicsShape->calculateLocalInertia(pDensity, inertia);

		btTransform& startTransform = btTransform();
		startTransform.setIdentity();
		startTransform.setOrigin(pLocation);

		btDefaultMotionState* physicsMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
			pDensity,			// mass
			physicsMotionState,	// initial position
			physicsShape,		// collision shape of body
			inertia				// local inertia
		);

		physicsRigidBody = new btRigidBody(rigidBodyCI);
		physicsRigidBody->setWorldTransform(startTransform);
		physicsRigidBody->setLinearVelocity(btVector3(0, 0, 0));
		physicsRigidBody->setAngularVelocity(btVector3(0, 0, 0));
		physicsRigidBody->setFriction(1.0);
		physicsRigidBody->activate(true);
		physicsRigidBody->setSleepingThresholds(0.f, 0.f); 
		if (userPtr) physicsRigidBody->setUserPointer(userPtr);

		//vMeshes.push_back(physicsMesh);
		vConvexMeshes.push_back(physicsShape);
		vCollisionShapes.push_back(physicsShape);
		vMotionStates.push_back(physicsMotionState);
		vRigidBodies.push_back(physicsRigidBody);

		mpDynamicsWorld->addRigidBody(physicsRigidBody);
	}
	else
	{
		for (int i = 0; i < mesh->size(); ++i)
		{
			btRigidBody* physicsRigidBody;
			int* indices = (int*)&mesh->at(i).indices[0];
			btScalar* vertices = &mesh->at(i).vertices[0].x;

			//btStridingMeshInterface* physicsMesh = new btTriangleIndexVertexArray(mesh->at(i).indices.size() / 3, indices, sizeof(int) * 3, mesh->at(i).vertices.size(), vertices, sizeof(btScalar) * 3);
			//
			//btCollisionShape* physicsShape = new btBvhTriangleMeshShape(physicsMesh, true, true);
			//physicsShape->setMargin(0.001); 
			//physicsShape->setLocalScaling(scale);
			btConvexHullShape* physicsShape = new btConvexHullShape((const btScalar*)(&mesh->at(i).vertices[0].x), mesh->at(i).vertices.size(), sizeof(btScalar) * 3);
			physicsShape->setMargin(0.001);
			physicsShape->setLocalScaling(scale);
			physicsShape->optimizeConvexHull();
			if (userPtr) physicsShape->setUserPointer(userPtr);
			//physicsShape->initializePolyhedralFeatures();

			btVector3 inertia(0.f, 0.f, 0.f);
			physicsShape->calculateLocalInertia(pDensity, inertia);

			btTransform& startTransform = btTransform();
			startTransform.setIdentity();
			startTransform.setOrigin(pLocation);

			btDefaultMotionState* physicsMotionState = new btDefaultMotionState(startTransform);
			btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
				pDensity,			// mass
				physicsMotionState,	// initial position
				physicsShape,		// collision shape of body
				inertia				// local inertia
			);

			physicsRigidBody = new btRigidBody(rigidBodyCI);
			physicsRigidBody->setWorldTransform(startTransform);
			physicsRigidBody->setLinearVelocity(btVector3(0, 0, 0));
			physicsRigidBody->setAngularVelocity(btVector3(0, 0, 0));
			physicsRigidBody->setFriction(1.0);
			physicsRigidBody->activate(true);
			physicsRigidBody->setSleepingThresholds(0.f, 0.f);
			if (userPtr) physicsRigidBody->setUserPointer(userPtr);

			//vMeshes.push_back(physicsMesh);
			vConvexMeshes.push_back(physicsShape);
			vCollisionShapes.push_back(physicsShape);
			vMotionStates.push_back(physicsMotionState);
			vRigidBodies.push_back(physicsRigidBody);

			mpDynamicsWorld->addRigidBody(physicsRigidBody);
		}
	}	
	mMutex->unlock();

	return vRigidBodies.back();
}

btRigidBody* BphysicsManager::createCompoundMeshBody(btVector3& pLocation, std::vector<Assimp3D>* mesh, float pDensity, btVector3& scale, void* userPtr)
{
	mMutex->lock();

	btCompoundShape* compShape = new btCompoundShape();
	btScalar* masses = new btScalar[mesh->size()];
	for (int i = 0; i < mesh->size(); ++i)
	{
		btRigidBody* physicsRigidBody;
		int* indices = (int*)&mesh->at(i).indices[0];
		btScalar* vertices = &mesh->at(i).vertices[0].x;

		//btStridingMeshInterface* physicsMesh = new btTriangleIndexVertexArray(mesh->at(i).indices.size() / 3, indices, sizeof(int) * 3, mesh->at(i).vertices.size(), vertices, sizeof(btScalar) * 3);
		//
		//btCollisionShape* physicsShape = new btBvhTriangleMeshShape(physicsMesh, true, true);
		//physicsShape->setMargin(0.001); 
		//physicsShape->setLocalScaling(scale);
		btConvexHullShape* physicsShape = new btConvexHullShape((const btScalar*)(&mesh->at(i).vertices[0].x), mesh->at(i).vertices.size(), sizeof(btScalar) * 3);
		physicsShape->setMargin(0.001);
		physicsShape->setLocalScaling(scale);
		physicsShape->optimizeConvexHull();
		if (userPtr) physicsShape->setUserPointer(userPtr);
		//physicsShape->initializePolyhedralFeatures();

		btVector3 inertia(0.f, 0.f, 0.f);
		physicsShape->calculateLocalInertia(pDensity, inertia);

		btTransform& startTransform = btTransform();
		startTransform.setIdentity();
		startTransform.setOrigin(pLocation);

		masses[i] = pDensity;

		//vMeshes.push_back(physicsMesh);
		vConvexMeshes.push_back(physicsShape);
		vCollisionShapes.push_back(physicsShape);
		//vMotionStates.push_back(physicsMotionState);

		compShape->addChildShape(startTransform, physicsShape);
	}
	btTransform& transform = btTransform();
	transform.setIdentity();
	transform.setOrigin(pLocation);

	btVector3 localInertia;
	compShape->calculateLocalInertia(pDensity, localInertia);

	compShape->calculatePrincipalAxisTransform(masses, transform, localInertia);
	
	btDefaultMotionState* physicsMotionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		pDensity,			// mass
		physicsMotionState,	// initial position
		compShape,			// collision shape of body
		localInertia		// local inertia
	);		

	btRigidBody::btRigidBodyConstructionInfo ci(pDensity, physicsMotionState, compShape, localInertia);
	ci.m_startWorldTransform.setOrigin(pLocation);

	btRigidBody* body = new btRigidBody(ci);//1,0,cyl0,localInertia);
	body->setWorldTransform(transform);
	body->setLinearVelocity(btVector3(0, 0, 0));
	body->setAngularVelocity(btVector3(0, 0, 0));
	body->setFriction(1.0);
	body->activate(true);
	body->setSleepingThresholds(0.f, 0.f);
	if (userPtr) body->setUserPointer(userPtr);
	mpDynamicsWorld->addRigidBody(body);
	vRigidBodies.push_back(body);

	delete[] masses;

	mMutex->unlock();

	return body;
}

void BphysicsManager::setScreenBoundBox()
{
	mMutex->lock();

	// world shape
	btCollisionShape* worldShape0 = new btBoxShape(btVector3(2000, 2000, 2000));//btStaticPlaneShape(btVector3(0, 1, 0), 0);
	btCollisionShape* worldShape1 = new btBoxShape(btVector3(2000, 2000, 2000));//btStaticPlaneShape(btVector3(0, -1, 0), 0);
	btCollisionShape* worldShape2 = new btBoxShape(btVector3(2000, 2000, 2000));//btStaticPlaneShape(btVector3(-1, 0, 0), 0);
	btCollisionShape* worldShape3 = new btBoxShape(btVector3(2000, 2000, 2000));//btStaticPlaneShape(btVector3(1, 0, 0), 0);

	vCollisionShapes.push_back(worldShape0);
	vCollisionShapes.push_back(worldShape1);
	vCollisionShapes.push_back(worldShape2);
	vCollisionShapes.push_back(worldShape3);

	btTransform t0;
	t0.setIdentity();
	t0.setOrigin(btVector3(0, -2000, 0)); // -2000 on y

	btTransform t1;
	t1.setIdentity();
	t1.setOrigin(btVector3(0, mGraphicsManager.getRenderHeight() + 2000, 0));

	btTransform t2;
	t2.setIdentity();
	t2.setOrigin(btVector3(mGraphicsManager.getRenderWidth() + 2000, 0, 0));

	btTransform t3;
	t3.setIdentity();
	t3.setOrigin(btVector3(-2000, 0, 0)); // -2000 on x

	btMotionState* motion0 = new btDefaultMotionState(t0);
	btRigidBody::btRigidBodyConstructionInfo info0(0.0, motion0, worldShape0);
	btRigidBody* body0 = new btRigidBody(info0);
	mpDynamicsWorld->addRigidBody(body0);

	vMotionStates.push_back(motion0);
	vRigidBodies.push_back(body0);

	btMotionState* motion1 = new btDefaultMotionState(t1);
	btRigidBody::btRigidBodyConstructionInfo info1(0.0, motion1, worldShape1);
	btRigidBody* body1 = new btRigidBody(info1);
	mpDynamicsWorld->addRigidBody(body1);

	vMotionStates.push_back(motion1);
	vRigidBodies.push_back(body1);

	btMotionState* motion2 = new btDefaultMotionState(t2);
	btRigidBody::btRigidBodyConstructionInfo info2(0.0, motion2, worldShape2);
	btRigidBody* body2 = new btRigidBody(info2);
	mpDynamicsWorld->addRigidBody(body2);

	vMotionStates.push_back(motion2);
	vRigidBodies.push_back(body2);

	btMotionState* motion3 = new btDefaultMotionState(t3);
	btRigidBody::btRigidBodyConstructionInfo info3(0.0, motion3, worldShape3);
	btRigidBody* body3 = new btRigidBody(info3);
	mpDynamicsWorld->addRigidBody(body3);

	vMotionStates.push_back(motion3);
	vRigidBodies.push_back(body3);

	mMutex->unlock();
}

void BphysicsManager::addConstraint(btTypedConstraint* p2p, bool chk)
{
	mMutex->lock();
	mpDynamicsWorld->addConstraint(p2p, chk);
	vConstrains.push_back(p2p);
	mMutex->unlock();
}

void BphysicsManager::removeConstraint(btTypedConstraint* constr)
{
	mMutex->lock();
	mpDynamicsWorld->removeConstraint(constr);
	std::vector<btTypedConstraint*>::iterator iter;
	for (iter = vConstrains.begin(); iter != vConstrains.end(); ++iter)
	{
		if (*iter == constr)
		{
			delete *iter;
			vConstrains.erase(iter);
			break;
		}
	}
	mMutex->unlock();
}