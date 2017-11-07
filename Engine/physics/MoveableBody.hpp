#ifndef MOVEABLEBODY_HPP
#define MOVEABLEBODY_HPP

#include "../Input/InputManager.hpp"
#include "PhysicsManager.hpp"
#include "../Tools/Types.hpp"
#include "../../sources/glm/glm.hpp"

class MoveableBody {
public:
	MoveableBody(InputManager& pInputManager, PhysicsManager& pPhysicsManager);
	b2Body* registerMoveableBody(glm::vec3& pLocation, int32_t pSizeX, int32_t pSizeY, int group, bool* action);
	void initialize();
	void update();
private:
	PhysicsManager& mPhysicsManager;
	InputManager& mInputManager;
	
	b2Body* mBody;
	b2MouseJoint* mTarget;

	
};
#endif