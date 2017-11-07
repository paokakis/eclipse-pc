#include "../../physics/BulletPhysicsManager.h"
#include "EventManager.hpp"
#include "..\..\tools\Log.hpp"
#include "../Engine/physics/BulletPhysicsManager.h"

EventManager* EventManager::mpInstance;

static void ScreenPosToWorldRay(
	float mouseX, float mouseY,             // Mouse position, in pixels, from bottom-left corner of the window
	float screenWidth, float screenHeight,  // Window size, in pixels
	glm::mat4 ViewMatrix,               // Camera position and orientation
	glm::mat4 ProjectionMatrix,         // Camera parameters (ratio, field of view, near and far planes)
	glm::vec3& out_origin,              // Ouput : Origin of the ray. /!\ Starts at the near plane, so if you want the ray to start at the camera's position instead, ignore this.
	glm::vec3& out_direction            // Ouput : Direction, in world space, of the ray that goes "through" the mouse.
) {

	// The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
	glm::vec4 lRayStart_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
		-1.0f, // The near plane maps to Z=-1 in Normalized Device Coordinates
		1.0f
	);
	glm::vec4 lRayEnd_NDC(
		((float)mouseX / (float)screenWidth - 0.5f) * 2.0f,
		((float)mouseY / (float)screenHeight - 0.5f) * 2.0f,
		1.f,
		1.0f
	);

	// Faster way (just one inverse)
	glm::mat4 M = glm::inverse(ProjectionMatrix * ViewMatrix);
	glm::vec4 lRayStart_world = M * lRayStart_NDC; 
	lRayStart_world /= lRayStart_world.w;
	glm::vec4 lRayEnd_world   = M * lRayEnd_NDC  ; 
	lRayEnd_world /= lRayEnd_world.w;

	glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
	lRayDir_world = glm::normalize(lRayDir_world);
	
	out_origin = glm::vec3(lRayStart_world);
	out_direction = glm::normalize(lRayDir_world);
}

static btRigidBody* m_pickedBody = NULL;
static btPoint2PointConstraint* m_pickedConstraint = NULL;
static btVector3 m_oldPickingPos, m_hitPos;
static btScalar m_oldPickingDist; int m_savedState;
static bool picked = false;


static btRigidBody* pickBody(glm::vec3& rayFromWorld, glm::vec3& rayToWorld)
{
	if (BphysicsManager::getInstance() == 0)
		return false;

	btVector3 hitPointWorld;
	btVector3 localPivot;
	btRigidBody* body;

	if (BphysicsManager::getInstance()->rayHasHit(rayFromWorld, rayToWorld, &hitPointWorld, &body))
	{
		if (body)
		{
			//other exclusions?
			if (!(body->isStaticObject() || body->isKinematicObject()))
			{
				m_pickedBody = body;
				m_savedState = m_pickedBody->getActivationState();
				m_pickedBody->setActivationState(DISABLE_DEACTIVATION);
				//printf("pickPos=%f,%f,%f\n",pickPos.getX(),pickPos.getY(),pickPos.getZ());
				localPivot = body->getCenterOfMassTransform().inverse() * hitPointWorld;
				btPoint2PointConstraint* p2p = new btPoint2PointConstraint(*body, localPivot);
				BphysicsManager::getInstance()->addConstraint(p2p, true);
				m_pickedConstraint = p2p;
				btScalar mousePickClamping = 1000.f;
				p2p->m_setting.m_impulseClamp = mousePickClamping;
				//very weak constraint for picking
				p2p->m_setting.m_tau = 0.1f;
			}
		}
		//pickObject(pickPos, rayCallback.m_collisionObject);
		m_oldPickingPos = btVector3(rayToWorld.x, rayToWorld.y, rayToWorld.z);
		m_hitPos = hitPointWorld;
		m_oldPickingDist = (hitPointWorld - btVector3(rayFromWorld.x, rayFromWorld.y, rayFromWorld.z)).length();

		picked = true;

		return body;
	}
	picked = false;

	return NULL;
}

static btRigidBody* movePickedBody(const btVector3& rayFromWorld, const btVector3& rayToWorld)
{
	if (m_pickedBody && m_pickedConstraint && picked)
	{
		btPoint2PointConstraint* pickCon = static_cast<btPoint2PointConstraint*>(m_pickedConstraint);
		if (pickCon)
		{
			//keep it at the same picking distance
			btVector3 dir = rayToWorld - rayFromWorld;
			dir.normalize();
			dir *= m_oldPickingDist;

			btVector3 newPivotB = rayFromWorld + dir;
			pickCon->setPivotB(newPivotB);

			return m_pickedBody;
		}
	}
	return NULL;
}

static btRigidBody* removePickingConstraint()
{
	if (m_pickedConstraint && picked)
	{
		btRigidBody* pickedBody;
		m_pickedBody->forceActivationState(m_savedState);
		m_pickedBody->activate();
		BphysicsManager::getInstance()->removeConstraint(m_pickedConstraint);
		pickedBody = m_pickedBody;
		m_pickedConstraint = 0;
		m_pickedBody = 0;
		//if (newPivotB.z() == localPivot.z()) return true;

		picked = false;

		return pickedBody;
	}

	return NULL;
}

EventManager::EventManager()
{
	Log::info("EventManager constructor");

	pSched = Scheduler::getInstance();
	pSched->registerTask(this, 10);
	pSched->startTask(this);
}

void EventManager::start()
{
	initialized = true;
}

EventManager::~EventManager()
{
	Log::info("EventManager destructor");
}

EventManager* EventManager::getInstance()
{
	if (mpInstance == NULL)
	{
		mpInstance = new EventManager;
	}

	return mpInstance;
}

void EventManager::addEvent(sEvent_t& _event)
{
	mMutex->lock();

	switch (_event.source)
	{
	case EVENT_MOUSE:
		// enqueue mouse event
		try 
		{
			mMouseEvQueue.push(reinterpret_cast<sMouseStatus_t*>(_event.data));
		}
		catch (const std::exception& e)
		{
			Log::error("EventManager mouse error casting data %s", e.what());
		}		
		break;
	case EVENT_KEYBOARD:
		// enqueue keyboard event
		break;
	case EVENT_TOUCH:
		break;
	case EVENT_JOYSTICK:
		break;
	default:
		break;
	}

	mMutex->unlock();
}

void EventManager::registerObject(sEventType_t const* eventLoc, MouseEventObject* mouseCBs)
{
	mMutex->lock();
	mMouseEventLoc.insert(std::make_pair(eventLoc, mouseCBs));
	mMutex->unlock();
}

void EventManager::unRegisterObject(sEventType_t const* pEventLoc)
{
	mMutex->lock();
	mMouseEventLoc.erase(pEventLoc);
	mMutex->unlock();
}

void EventManager::registerObject(KeyboardEventObject* keyboardCBs)
{
	mMutex->lock();
	mKeyEventObjs.push_back(keyboardCBs);
	mMutex->unlock();
}

void EventManager::unRegisterObject(KeyboardEventObject* pObj)
{
	mMutex->lock();
	for (mKeyEventIter = mKeyEventObjs.begin(); mKeyEventIter != mKeyEventObjs.end(); ++mKeyEventIter)
	{
		if ((*mKeyEventIter) == pObj)
		{
			mKeyEventObjs.erase(mKeyEventIter);

			break;
		}
	}
	mMutex->unlock();
}

void EventManager::taskRun()
{
	if (initialized)
	{
		static float prevScrollx = 0, prevScrolly = 0;
		static GraphicsManager* pGraphMan = GraphicsManager::getInstance();

		// process mouse events
		while (!mMouseEvQueue.empty())
		{
			static sMouseStatus_t* mouseEvent;
			mouseEvent = mMouseEvQueue.front();
			mMouseEvQueue.pop();

			for (mMouseEventIter = mMouseEventLoc.begin(); mMouseEventIter != mMouseEventLoc.end(); ++mMouseEventIter)
			{
				if (*(*mMouseEventIter).first == MOUSE_PICKING || *(*mMouseEventIter).first == MOUSE_PICKING_NO_DRAG)
				{
					static bool firstClick = true;
					static btRigidBody* body;
					// PICKING IS DONE HERE
					glm::vec3 out_origin;
					glm::vec3 out_direction;
					ScreenPosToWorldRay(
						mouseEvent->pos.x,
						mouseEvent->pos.y,
						pGraphMan->getRenderWidth(),
						pGraphMan->getRenderHeight(),
						pGraphMan->getViewMatrix(),
						pGraphMan->getProjection(),
						out_origin,
						out_direction
					);

					glm::vec3 out_end = out_origin + (out_direction * 1000.f);

					if (firstClick && (mouseEvent->leftButtonDown))
					{
						body = pickBody(out_origin, out_end);
						firstClick = false;
						(*mMouseEventIter).second->onLeftClick(mouseEvent->pos);
						(*mMouseEventIter).second->onLeftBodyPicked(mouseEvent->pos, body);
					}

					if (!firstClick && (mouseEvent->leftButtonDown) && *(*mMouseEventIter).first != MOUSE_PICKING_NO_DRAG)
					{
						body = movePickedBody(btVector3(out_origin.x, out_origin.y, out_origin.z), btVector3(out_end.x, out_end.y, out_end.z));
						(*mMouseEventIter).second->onLeftDrag(mouseEvent->pos);
						(*mMouseEventIter).second->onLeftBodyDraged(mouseEvent->pos, body);
					}

					if (!firstClick && (mouseEvent->leftButtonUp))
					{
						firstClick = true;
						
						if (body = removePickingConstraint())
						{
							(*mMouseEventIter).second->onLeftRelease(mouseEvent->pos);
							(*mMouseEventIter).second->onLeftBodyReleased(mouseEvent->pos, body);
						}
					}

					(*mMouseEventIter).second->onHover(mouseEvent->pos);
				}
				else if (*(*mMouseEventIter).first == MOUSE_FREEMOVE)
				{
					if (prevScrollx != mouseEvent->scrollx || prevScrolly != mouseEvent->scrolly)
					{
						prevScrollx = mouseEvent->scrollx;
						prevScrolly = mouseEvent->scrolly;
						(*mMouseEventIter).second->onScroll(prevScrollx, prevScrolly);
					}
					else if (mouseEvent->leftButtonDown) (*mMouseEventIter).second->onLeftClick(mouseEvent->pos);
					else if (mouseEvent->leftButtonUp) (*mMouseEventIter).second->onLeftRelease(mouseEvent->pos);
					else if (mouseEvent->middleButtonDown) (*mMouseEventIter).second->onMidClick(mouseEvent->pos);
					else if (mouseEvent->middleButtonUp) (*mMouseEventIter).second->onMidRelease(mouseEvent->pos);
					else if (mouseEvent->rightButtonDown) (*mMouseEventIter).second->onRightClick(mouseEvent->pos);
					else if (mouseEvent->rightButtonUp) (*mMouseEventIter).second->onRightRelease(mouseEvent->pos);
					else (*mMouseEventIter).second->onHover(mouseEvent->pos);
				}
				else if (MOUSE_NO_PICK_NO_DRAG == *(*mMouseEventIter).first)
				{
					// Do Nothing
				}
				else {
					// Do nothing
				}
			}
		}

		// process keyboard events
		for (mKeyEventIter = mKeyEventObjs.begin(); mKeyEventIter != mKeyEventObjs.end(); ++mKeyEventIter)
		{
			for (int i = 0; i < (*mKeyEventIter)->mRegisteredKeys.size(); ++i)
			{
				unsigned int codePoint = 0;
				if (Keyboard::keyDown((*mKeyEventIter)->mRegisteredKeys[i]))
				{
					(*mKeyEventIter)->keyboardDown((*mKeyEventIter)->mRegisteredKeys[i]);
				}
				if (Keyboard::keyUp((*mKeyEventIter)->mRegisteredKeys[i]))
				{
					(*mKeyEventIter)->keyboardUp((*mKeyEventIter)->mRegisteredKeys[i]);
				}
				if (Keyboard::key((*mKeyEventIter)->mRegisteredKeys[i]))
				{
					(*mKeyEventIter)->keyboardPressed((*mKeyEventIter)->mRegisteredKeys[i]);
				}
				if ((codePoint = Keyboard::getCodePoint()) != 0)
				{
					(*mKeyEventIter)->KeyboardString(codePoint);
				}
			}
		}
	}
}