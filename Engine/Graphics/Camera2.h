#ifndef CAMERA2_H
#define CAMERA2_H
#include "../Engine/Graphics/GUI/EventManager.hpp"
#include "../TimeManager.hpp"

class Camera2 : public MouseEventObject
{
public:
	Camera2(TimeManager& timeMan, float width, float height, float depth);
	~Camera2();

	void initialize();
	void update();

	void setCamFront(glm::vec3& pos)
	{
		cameraFront = glm::radians(pos);
		upDateView();
		upDateMVP();
	}
	void setCamPosition(glm::vec3& pos) 
	{
		cameraPos = pos;
		upDateView();
		upDateMVP();
	}
	void setCamTarget(glm::vec3& targ) 
	{ 
		cameraTarget = targ; 
		upDateView();
		upDateMVP();
	}
	void setCamUp(glm::vec3& _up)
	{
		worldUp = _up;
		upDateView();
		upDateMVP();
	}
	void setCamYawPitch(float yaw, float pitch) 
	{ 
		this->yaw = yaw; 
		this->pitch = pitch; 

		upDateFront();
		upDateView();
		upDateMVP();
	}
	void rotateAroundPos(const glm::vec3& objPos, float radius, float height)
	{
		cameraPos = objPos + (radius * cos(mTimeManager.elapsedTotal()), height, radius * sin(mTimeManager.elapsedTotal()));
		upDateView();
		upDateMVP();
	}
	void lookAt(const glm::vec3& pos);

	void setCamDefault();

	float* getProjectionMatrix() { return &MVP[0][0]; }
	glm::mat4& getMVP() { return MVP; }
	glm::mat4& getProjection() { return projection; }
	glm::mat4& getOrthoMVP() { return MVPortho; }
	glm::mat4& getPerspectiveProjection() { return MVPpersp; }
	glm::mat4& getViewMatrix() { return view; }
	glm::vec3& getCameraRight() { return cameraRight; }
	glm::vec3& getCameraUp() { return cameraUp; }
	glm::vec3& getCamTarget() { return cameraTarget; }
	glm::vec3& getCamPosition() { return cameraPos; }
	glm::vec3& getCamCorshair() { return mTargetPos; }
	float getFOV() { return fov; }

	void setMouseMove(bool val) { mouseMove = val; }

protected:

	void onHover(const glm::vec2&);
	void onLeftClick(const glm::vec2&);
	void onLeftRelease(const glm::vec2&);
	void onRightClick(const glm::vec2&);
	void onScroll(float x, float y);
private:
	TimeManager& mTimeManager;
	EventManager* pEventManager;

	float mWidth; float mHeight; float mDepth;

	sEventType_t mMouseEventsLoc;

	glm::vec3 mTargetPos;
	float targetSensitivity = 7.f;

	glm::vec3 cameraPos;
	glm::vec3 cameraTarget;
	glm::vec3 cameraDirection;
	glm::vec3 worldUp;
	glm::vec3 cameraRight;
	glm::vec3 cameraUp;
	glm::vec3 cameraFront;

	glm::mat4 view;
	glm::mat4 projection;
	glm::mat4 model;
	glm::mat4 MVP;
	glm::mat4 MVPortho;
	glm::mat4 MVPpersp;
	float fov;
	float Z;
	float cameraSpeed;

	bool firstMouse = true;
	bool mouseMove = false;
	float yaw = 0;
	float pitch = 0;
	float sensitivity;
	float xoffset;
	float yoffset;
	float distance = 100;

	void upDateProjection();
	void upDateModel();
	void upDateView();
	void upDateQuatView(glm::vec3& quat);
	void upDateMVP();
	void setProjections();
	void upDateFront();
};

#endif