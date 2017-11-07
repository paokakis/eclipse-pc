#include "Keyboard.h"

bool Keyboard::keys[GLFW_KEY_LAST] = { 0 };
bool Keyboard::keysDown[GLFW_KEY_LAST] = { 0 };
bool Keyboard::keysUp[GLFW_KEY_LAST] = { 0 };
std::vector<unsigned int> Keyboard::codePointVec = { 0 };

void Keyboard::keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
	if (key < 0)
		return;

	if (action != GLFW_RELEASE && keys[key] == false)
	{
		keysDown[key] = true;
		keysUp[key] = false;
	}
	if (action == GLFW_RELEASE && keys[key] == true)
	{
		keysDown[key] = false;
		keysUp[key] = true;
	}

	keys[key] = action != GLFW_RELEASE;
}

void Keyboard::character_callback(GLFWwindow* window, unsigned int codepoint)
{
	codePointVec.push_back(codepoint);
}

bool Keyboard::keyDown(int key)
{
	bool x = keysDown[key];
	keysDown[key] = false;
	return x;
}

bool Keyboard::keyUp(int key)
{
	bool x = keysUp[key];
	keysUp[key] = false;
	return x;
}

bool Keyboard::key(int key)
{
	return keys[key];
}

unsigned int Keyboard::getCodePoint()
{
	unsigned int retVal = 0;

	if (codePointVec.size() > 0)
	{
		retVal = codePointVec.front();
		codePointVec.erase(codePointVec.begin());
	}

	return retVal;
}