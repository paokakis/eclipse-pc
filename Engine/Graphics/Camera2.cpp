#include "Camera2.h"
#include "../tools/Log.hpp"
#include "GLFW\glfw3.h"
#include <glm.hpp>
#include "glm\gtc\quaternion.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define CAMERA_SPEED		(100.0)
#define MOUSE_SENSITIVITY	(0.1)
#define FOV					(45.0f)

Camera2::Camera2(TimeManager& timeMan, float width, float height, float depth) :
	mTimeManager(timeMan), mWidth(width), mHeight(height), mDepth(depth), yaw(0.f), pitch(0.f), mTargetPos(0), xoffset(0), yoffset(0)
{
	Log::info("Camera2 constructor");
	setCamDefault();
}

void Camera2::initialize()
{
	Log::info("Camera2 init/update");
	
	// mouse setup
	pEventManager = EventManager::getInstance();
	mMouseEventsLoc = MOUSE_FREEMOVE;
	pEventManager->registerObject(&mMouseEventsLoc, this);
}

void Camera2::setProjections()
{
	glm::vec3 _camPos_;
	glm::vec3 _lookPos_;
	glm::vec3 _headPos_;
	glm::mat4 _project_;
	glm::mat4 _view_;
	glm::mat4 _model_;

	_project_ = glm::ortho(
		0.0f, mWidth,
		0.0f, mHeight, 
		0.1f, mDepth);

	_camPos_ = glm::vec3(0, 0, mDepth);
	_lookPos_ = glm::vec3(0, 0, 0);
	_headPos_ = glm::vec3(0, 1, 0);

	_view_ = glm::lookAt(
		_camPos_, // camera position
		_lookPos_, // look at origin
		_headPos_	// Head is up
	);

	_model_ = glm::mat4(1.0f);
	MVPortho = _project_ * _view_ * _model_;

	Z = mDepth / (2 * (float)tan(glm::radians(fov) / 2.0));

	_project_ = glm::perspective(
		glm::radians(fov), 
		mWidth / mHeight,
		0.1f, mDepth);

	_camPos_ = glm::vec3(mWidth / 2, mHeight / 2, Z);
	_lookPos_ = glm::vec3(mWidth / 2, mHeight / 2, 0);
	_headPos_ = glm::vec3(0, 1, 0);

	_view_ = glm::lookAt(
		_camPos_,	// camera position
		_lookPos_,	// look at origin
		_headPos_	// Head is up
	);

	_model_ = glm::mat4(1.0f);
	MVPpersp = _project_ * _view_ * _model_;
}

void Camera2::update()
{
	cameraSpeed = CAMERA_SPEED * mTimeManager.elapsed();

	mTargetPos.x = cameraPos.x - (xoffset * targetSensitivity); // cameraPos.x; //xoffset * targetSensitivity;
	mTargetPos.y = cameraPos.y + (yoffset * targetSensitivity); //yoffset * targetSensitivity;
	mTargetPos.z = cameraPos.z + 800.f;//cameraPos.z;

	//Log::info("Camera target x %f , y %f", mTargetPos.x, mTargetPos.y);
}

Camera2::~Camera2()
{
	pEventManager->unRegisterObject(&mMouseEventsLoc);
	//pEventManager->unRegisterObject(this);
}

void Camera2::onHover(const glm::vec2& pos)
{
	//mTargetPos.x = pos.x 
	//mTargetPos.z = cameraPos.z + 150.f;
}

void Camera2::onRightClick(const glm::vec2& pos)
{
	if (mouseMove)
	{
		//Log::info("onHover");
		static float lastX;
		static float lastY;
		static float Xoffset;
		static float Yoffset;

		if (firstMouse)
		{
			lastX = pos.x;
			lastY = pos.y;
			firstMouse = false;
		}
		xoffset = lastX - pos.x;
		yoffset = pos.y - lastY;
		lastX = pos.x;
		lastY = pos.y;

		Xoffset = xoffset * mTimeManager.elapsed();
		Yoffset = yoffset * mTimeManager.elapsed();

		yaw += Xoffset;
		pitch += Yoffset;

		if (1)
		{
			if (pitch > 89.0f)
			{
				pitch = 89.0f;
			}
			if (pitch < -89.0f)
			{
				pitch = -89.0f;
			}
		}

		upDateFront();
		upDateView();
		upDateMVP();
	}
}

void Camera2::onLeftClick(const glm::vec2& pos)
{
	//Log::info("On left click %f,%f", pos.x, pos.y);
}

void Camera2::onLeftRelease(const glm::vec2& pos)
{

}

// Zoom
void Camera2::onScroll(float x, float y)
{
	static float prevY = 0;
	float Y = y - prevY;
	prevY = y;

	if (fov >= 1.0f && fov <= 45.0f)
		fov -= Y;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;

	upDateProjection();
	upDateMVP();
}

void Camera2::upDateProjection()
{
	projection = glm::perspective(
		glm::radians(fov),
		mWidth / mHeight,
		0.1f, mDepth);
}

void Camera2::upDateModel()
{
	model = glm::mat4(1.0f);
}

void Camera2::upDateView()
{
	cameraDirection = glm::normalize(cameraPos - cameraTarget);
	cameraRight = glm::normalize(glm::cross(cameraDirection, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

	view = glm::lookAt(
		cameraPos,
		cameraPos + cameraFront,
		cameraUp);
}

void Camera2::upDateQuatView(glm::vec3& quat)
{
	static float key_pitch;	
	static float key_yaw;
	static float key_roll;

	key_pitch = quat.x;
	key_yaw = quat.y;
	key_roll = quat.z;

	//temporary frame quaternion from pitch,yaw,roll 
	//here roll is not used
	glm::quat key_quat = glm::quat(glm::vec3(key_pitch, key_yaw, key_roll));
	//reset values
	key_pitch = key_yaw = key_roll = 0;

	//order matters,update camera_quat
	glm::quat camera_quat = key_quat * camera_quat;
	camera_quat = glm::normalize(camera_quat);
	glm::mat4 rotate = glm::mat4_cast(camera_quat);

	glm::mat4 translate = glm::mat4(1.0f);
	translate = glm::translate(translate, -cameraPos);

	MVP = rotate * translate;



	////FPS camera:  RotationX(pitch) * RotationY(yaw)
	//glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));
	//glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
	//glm::quat qRoll = glm::angleAxis(roll, glm::vec3(0, 0, 1));

	////For a FPS camera we can omit roll
	//glm::quat orientation = qPitch * qYaw;
	//orientation = glm::normalize(orientation);
	//glm::mat4 rotate = glm::mat4_cast(orientation);

	//glm::mat4 translate = glm::mat4(1.0f);
	//translate = glm::translate(translate, -cameraPos);

	//MVP = rotate * translate;
}

void Camera2::lookAt(const glm::vec3& pos)
{
	cameraDirection = glm::normalize(cameraPos - pos);
	cameraRight = glm::normalize(glm::cross(cameraDirection, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraDirection));

	view = glm::lookAt(
		cameraPos,
		pos,
		cameraUp);

	upDateMVP();
}

void Camera2::upDateMVP()
{
	MVP = projection * view * model;
}

void Camera2::setCamDefault()
{
	setProjections();

	fov = FOV;

	cameraPos = glm::vec3(mWidth / 2, mHeight / 2, mDepth);
	cameraTarget = glm::vec3(mWidth / 2, mHeight / 2, 0);
	cameraFront = glm::vec3(yaw, pitch, -1.0f);
	worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	sensitivity = MOUSE_SENSITIVITY;

	upDateProjection();
	upDateView();
	upDateModel();
	upDateMVP();
}

void Camera2::upDateFront()
{
	static glm::vec3 front;

	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.z = sin(glm::radians(pitch));
	front.y = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}