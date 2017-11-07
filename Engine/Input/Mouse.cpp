#include "Mouse.h"

float Mouse::x = 0;
float Mouse::y = 0;
float Mouse::scrollX = 0;
float Mouse::scrollY = 0;

bool Mouse::buttons[GLFW_MOUSE_BUTTON_LAST] = { 0 };
bool Mouse::buttonsDown[GLFW_MOUSE_BUTTON_LAST] = { 0 };
bool Mouse::buttonsUp[GLFW_MOUSE_BUTTON_LAST] = { 0 };
bool Mouse::mShow = true;
bool Mouse::mMouseLocked = false;

void Mouse::mousePosCallback(GLFWwindow* window, double _x, double _y)
{
	int width, height;

	glfwGetFramebufferSize(window, &width, &height);
	x = (float)_x;
	y = height - (float)_y;
}

void Mouse::mouseButtonsCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button < 0)
		return;

	if (action != GLFW_RELEASE && buttons[button] == false) 
	{
		buttonsDown[button] = true;
		buttonsUp[button] = false;
	}

	if (action == GLFW_RELEASE && buttons[button] == true)
	{
		buttonsDown[button] = false;
		buttonsUp[button] = true;
	}

	buttons[button] = action != GLFW_RELEASE;
}

void Mouse::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollX += static_cast<float>(xoffset);
	scrollY += static_cast<float>(yoffset);
}

float Mouse::getMouseX()
{
	return x;
}

float Mouse::getMouseY()
{
	return y;
}

bool Mouse::buttonDown(int button)
{
	bool x = buttonsDown[button];
	buttonsDown[button] = false;
	return x;
}

bool Mouse::buttonUp(int button)
{
	bool x = buttonsUp[button];
	buttonsUp[button] = false;
	return x;
}

bool Mouse::button(int button)
{
	return buttons[button];
}

void Mouse::show(GLFWwindow* window, bool show)
{
	mShow = show;
	if (mShow)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetCursorPos(window, 1280 / 2, 720 / 2);
	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(window, 1280 / 2, 720 / 2);
	}
}

float Mouse::getMouseScrollX()
{ 
	return scrollX; 
}

float Mouse::getMouseScrollY()
{
	return scrollY;
}