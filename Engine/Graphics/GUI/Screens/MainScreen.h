#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H
#include "..\..\GraphicsManager.hpp"
#include "..\..\..\Engine\TimeManager.hpp"
#include "..\..\Engine\Input\InputManager.hpp"
#include "..\..\Engine\physics\BulletPhysicsManager.h"
#include "..\..\Engine\physics\BulletMoveableBody.h"
#include "..\..\Text2D.h"
#include "..\EventManager.hpp"
#include "..\..\..\Game\common\effects\HQ\Sky.h"
#include <vector>
#include "..\..\Object3D.h"

class MainScreen : public MouseEventObject
{
public:
	MainScreen(GraphicsManager& graphMan, TimeManager& timeMan, InputManager& inpMan, BphysicsManager& mBphysics);
	~MainScreen();

	void initialize();

	status screenLoad();
	void screenUpdate();
	void screenDraw();
	bool screenDrawFinished();

private:
	struct Vertex {
		GLfloat x, y, z;
	};
	sEventType_t mMouseEventsLoc;

	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	InputManager& mInputManager;
	EventManager* mEventManager;
	BphysicsManager& mBphysicsManager;
	BmoveableBody mStartMoveableBody;

	glm::vec3 mCubePos;
	glm::vec3 mCubeScale;
	Text2D mCubeText;
	float mCubeSize;
	Sky mSky;

	Object3D* mCube;
	glm::vec4 mCubeTextColor;
	glm::vec4 mCubeBackColor;
	glm::vec3 mCubeTextPos;
	std::vector<TextureProperties*> mCubeTexVec;

	GLuint mShaderProgram, aPosition, uProjection, uColor;

	float mTimeout;
	bool mNextScreen;
private:
	void prepVrtx(Vertex pVertices[4]);

protected:
	void onHover(const glm::vec2&);
	void onLeftClick(const glm::vec2&);
	void onLeftDrag(const glm::vec2&);
	void onLeftRelease(const glm::vec2&);
};


#endif // !MAIN_SCREEN_H