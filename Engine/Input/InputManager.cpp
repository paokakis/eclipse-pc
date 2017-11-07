#include "InputManager.hpp"
#include "../Tools/Log.hpp"
#include <cmath>
#include "Keyboard.h"
#include "Mouse.h"


InputManager::InputManager(GraphicsManager& pGraphicsManager) :
	mGraphicsManager(pGraphicsManager)
{
	Log::info("InputManager constructor");
	mEventManager = EventManager::getInstance();
}

void InputManager::start() 
{
	Log::info("Starting InputManager.");
	mQuit = false; 

	setMouseShow(false);
}

bool InputManager::onKeyboardEvent() 
{
	mQuit = false;

	if (Keyboard::keyDown(GLFW_KEY_ESCAPE))
	{
		mQuit = true;
	}

	return true;
}

bool InputManager::onMouseEvent()
{
	mMouseStatus.pos.x = Mouse::getMouseX();
	mMouseStatus.pos.y = Mouse::getMouseY();
	mMouseStatus.leftButtonDown = Mouse::buttonDown(GLFW_MOUSE_BUTTON_LEFT);
	mMouseStatus.leftButtonUp = Mouse::buttonUp(GLFW_MOUSE_BUTTON_LEFT);
	mMouseStatus.middleButtonDown = Mouse::buttonDown(GLFW_MOUSE_BUTTON_MIDDLE);
	mMouseStatus.middleButtonUp = Mouse::buttonUp(GLFW_MOUSE_BUTTON_MIDDLE);
	mMouseStatus.rightButtonDown = Mouse::buttonDown(GLFW_MOUSE_BUTTON_RIGHT);
	mMouseStatus.rightButtonUp = Mouse::buttonUp(GLFW_MOUSE_BUTTON_RIGHT);
	mMouseStatus.scrollx = Mouse::getMouseScrollX();
	mMouseStatus.scrolly = Mouse::getMouseScrollY();

	mEvent.source = EVENT_MOUSE;
	mEvent.data = &mMouseStatus;

	mEventManager->addEvent(mEvent);

	return true;
}

void InputManager::update()
{
	mQuit |= glfwWindowShouldClose(mGraphicsManager.getDisplay());

	if (mMouseLocked)
	{
		glfwSetCursorPos(
			mGraphicsManager.getDisplay(),
			mGraphicsManager.getRenderWidth() / 2,
			mGraphicsManager.getRenderHeight() / 2);
	}
}