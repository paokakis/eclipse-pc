#ifndef MAIN_SCREEN1_H
#define MAIN_SCREEN1_H
#include "..\..\GraphicsManager.hpp"
#include "..\..\..\Engine\TimeManager.hpp"
#include "..\..\Engine\Input\InputManager.hpp"
#include "..\..\Engine\physics\BulletPhysicsManager.h"
#include "..\..\Engine\physics\BulletMoveableBody.h"
#include "..\..\Text2D.h"
#include "..\EventManager.hpp"
//#include "../Engine/Graphics/SkyBox.h"
#include "../Game/common/effects/HQ/Sky.h"
#include <vector>
#include "..\..\Object3D.h"
#include "../Engine/Graphics/Text2D.h"
#include "../CedGUI.h"

class MainScreen : public KeyboardEventObject, public MouseEventObject
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
	void onTestButtonClickEvent();
	struct Vertex {
		GLfloat x, y, z;
	};
	sEventType_t mMouseEventsLoc;

	GraphicsManager& mGraphicsManager;
	TimeManager& mTimeManager;
	InputManager& mInputManager;
	EventManager* mEventManager;
	BphysicsManager& mBphysicsManager;

	CedGUI* mp_GUI;
	Text2D mText;
	Text2D mCreditsText;
	const float mCreditsTextSpeed = 50;

	glm::vec3 mCreditsTextPos;
	bool mCreditsEnabled = false;

	glm::vec3 mCubeStartPos;
	glm::vec3 mCubeStartScale;
	glm::vec3 mCubeOptionsPos;
	glm::vec3 mCubeOptionsScale;
	glm::vec3 mCubeCreditsPos;
	glm::vec3 mCubeCreditsScale;
	glm::vec3 mCubeQuitPos;
	glm::vec3 mCubeQuitScale;
	glm::vec3 mEarthPos;
	glm::vec3 mEarthScale;
	glm::vec3 mMoonPos;
	glm::vec3 mMoonScale;
	glm::vec3 mMenuPos;
	glm::vec3 mMenuScale;
	glm::vec3 mTextPos;
	glm::vec4 mTextColor;

	Text2D mCubeText;
	float mCubeSize;
	Sky* mSky;

	Object3D* mStartCube; BmoveableBody mStartMoveableBody;
	Object3D* mOptionsCube; BmoveableBody mOptionsMoveableBody;
	Object3D* mCreditsCube; BmoveableBody mCreditsMoveableBody;
	Object3D* mQuitCube; BmoveableBody mQuitMoveableBody;
	Object3D* mEarthObj; BmoveableBody mEarthMoveableBody;
	Object3D* mMoonObj;	BmoveableBody mMoonMoveableBody;
	
	Object3D* mMenuPanel; //BmoveableBody mMenuMoveableBody;

	glm::vec4 mCubeTextColor;
	glm::vec4 mCubeBackColor;
	glm::vec4 mCubeSelectedColor;
	glm::vec4 mCubePressedColor;
	glm::vec3 mCubeTextPos;
	bool mMenuEnabled = false;
	std::vector<TextureProperties*> mCubeTexVec;

	GLuint mShaderProgram, aPosition, uProjection, uColor;

	float mTimeout;
	bool mNextScreen;

	const float mKeyTime = 0.2f;
	float mKeyCoolDown = 0;

private:
	void prepVrtx(Vertex pVertices[4]);

protected:
	void onHover(const glm::vec2&);
	void onLeftBodyPicked(const glm::vec2&, btRigidBody* body);
	void onLeftBodyReleased(const glm::vec2&, btRigidBody* body);

	void keyboardPressed(int key);
	void keyboardUp(int key);
};


#endif // !MAIN_SCREEN_H
