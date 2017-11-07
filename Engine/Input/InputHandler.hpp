#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

class InputHandler {
public:
	virtual ~InputHandler() {};
	virtual bool onKeyboardEvent() = 0;
	virtual bool onMouseEvent() = 0;

	bool mQuit = false;
};

#endif