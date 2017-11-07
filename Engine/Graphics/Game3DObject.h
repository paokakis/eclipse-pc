#ifndef GAME_3D_OBJECT_H
#define GAME_3D_OBJECT_H
#include "GraphicsManager.hpp"
#include "Object3D1.h"
#include "../physics/BulletMoveableBody.h"

class Game3DObject : public GraphicsComponent
{
public:
	Game3DObject(GraphicsManager& graphMan, TimeManager& timeMan, BphysicsManager& physMan) : 
		mGraphicsManager(graphMan), mTimeManager(timeMan), mPhysicsManager(physMan)
	{

	}
	~Game3DObject() {}

	status registerGameObj(glm::vec3& pos, glm::vec3& scale,
		const std::string& objPath, const std::string& texturePath, Object3D_Shader_TYPE type,
		const std::string& textureNormalPath = "", bool enBillBoard = false)
	{
		objects_t* obj3D = new objects_t();
		
		obj3D->pGraphObj = new Object3D1(mGraphicsManager, mTimeManager, pos, scale, objPath, texturePath, type, textureNormalPath, enBillBoard);
		obj3D->pMoveableBody = new BmoveableBody(mPhysicsManager);
		obj3D->pGraphObj->load();

		objects.push_back(obj3D);
	}

protected:
	status load();
	void draw();
	void update();
	void drawShadows() {}
private:
	GraphicsManager& mGraphicsManager; 
	TimeManager& mTimeManager;
	BphysicsManager& mPhysicsManager;

	typedef struct obj_3D
	{
		Object3D1* pGraphObj;
		BmoveableBody* pMoveableBody;
	} objects_t;

	std::vector<objects_t*>::iterator objIter;
	std::vector<objects_t*> objects;
};

#endif