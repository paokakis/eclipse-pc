#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "GLFW\glfw3.h"
#include <vector>

class Keyboard;

class KeyboardEventObject {
	friend class EventManager;
public:
	virtual ~KeyboardEventObject() { mRegisteredKeys.clear(); }
protected:
	void registerKey(int key)
	{
		mRegisteredKeys.push_back(key);
	}
	virtual void keyboardDown(int key) {}
	virtual void keyboardUp(int key) {}
	virtual void keyboardPressed(int key) {}
	virtual void KeyboardString(unsigned int code) {}
private:
	std::vector<int> mRegisteredKeys;
};


class Keyboard
{
public:
	static void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
	static void character_callback(GLFWwindow* window, unsigned int codepoint);

	static bool keyDown(int key);
	static bool keyUp(int key);
	static bool key(int key);
	static unsigned int getCodePoint();
private:
	static bool keys[];
	static bool keysUp[];
	static bool keysDown[];
	static std::vector<unsigned int> codePointVec;
};


#endif // KEYBOARD_H
