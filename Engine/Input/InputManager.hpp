#ifndef _PACKT_INPUTMANAGER_HPP_
#define _PACKT_INPUTMANAGER_HPP_

#include "../Graphics/GraphicsManager.hpp"
#include "InputHandler.hpp"
#include "../Tools/Types.hpp"
#include "../Tools/Configuration.hpp"
#include "../../sources/glm/glm.hpp"
#include "../Input/Mouse.h"
#include "../Graphics/GUI/EventManager.hpp"

class InputManager : public InputHandler {
public:
	InputManager(GraphicsManager& pGraphicsManager);

	void setQuit(bool val) { mQuit = val; }
	bool getQuit() { return (mQuit); }

	void setMouseShow(bool state) { Mouse::show(mGraphicsManager.getDisplay(), state); }
	void setMouseLocked(bool state) { mMouseLocked = state; }
		
	void start();
	void update();
protected:
	bool onKeyboardEvent();
	bool onMouseEvent();
private:
	GraphicsManager& mGraphicsManager;
	EventManager* mEventManager;
	// event data
	sMouseStatus_t mMouseStatus;
	sEvent_t mEvent;	
	// Input values.
	float mScaleFactor;

	bool mMouseLocked = false;
};

#endif