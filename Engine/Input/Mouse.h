#ifndef ENGINE_MOUSE
#define ENGINE_MOUSE
#include "glm.hpp"
#include <GLFW\glfw3.h>
#include <btBulletCollisionCommon.h>

typedef struct {
	glm::vec2 pos;
	bool leftButtonDown;
	bool leftButtonUp;
	bool rightButtonDown;
	bool rightButtonUp;
	bool middleButtonDown;
	bool middleButtonUp;

	float scrollx = 0.f;
	float scrolly = 0.f;
} sMouseStatus_t;

// to be used by application
typedef enum {
	MOUSE_FREEMOVE,
	MOUSE_PICKING,
	MOUSE_PICKING_NO_DRAG,
	MOUSE_NO_PICK_NO_DRAG
} sEventType_t;

class MouseEventObject {
	friend class EventManager;
public:
	virtual ~MouseEventObject() {}
protected:
	virtual void onHover(const glm::vec2&) {}
	virtual void onLeftClick(const glm::vec2&) {}
	virtual void onLeftDrag(const glm::vec2&) {}
	virtual void onLeftRelease(const glm::vec2&) {}
	virtual void onMidClick(const glm::vec2&) {}
	virtual void onMidRelease(const glm::vec2&) {}
	virtual void onRightClick(const glm::vec2&) {}
	virtual void onRightRelease(const glm::vec2&) {}
	virtual void onDefault(const glm::vec2&) {}
	virtual void onScroll(float x, float y) {}

	virtual void onLeftBodyPicked(const glm::vec2&, btRigidBody* body) {}
	virtual void onLeftBodyDraged(const glm::vec2&, btRigidBody* body) {}
	virtual void onLeftBodyReleased(const glm::vec2&, btRigidBody* body) {}
};

class Mouse {
public:
	static void mousePosCallback(GLFWwindow* window, double _x, double _y);
	static void mouseButtonsCallback(GLFWwindow* window, int button, int action, int mods);
	static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	static float getMouseX();
	static float getMouseY();

	static float getMouseScrollX();
	static float getMouseScrollY();

	static bool buttonDown(int button);
	static bool buttonUp(int button);
	static bool button(int button);

	static void show(GLFWwindow* window, bool show);
private:
	static float x;
	static float y;

	static bool buttons[];
	static bool buttonsDown[];
	static bool buttonsUp[];

	static float scrollX;
	static float scrollY;

	static bool mShow;
	static bool mMouseLocked;
};


#endif // ENGINE_MOUSE
