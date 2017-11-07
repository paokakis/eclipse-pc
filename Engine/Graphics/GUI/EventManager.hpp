#pragma once
#include "..\..\Input\Mouse.h"
#include "..\..\Input\Keyboard.h"
#include <queue>
#include <map>
#include "../OS/Scheduler.h"

// to be used by engine
typedef enum {
	EVENT_MOUSE,
	EVENT_KEYBOARD,
	EVENT_TOUCH,
	EVENT_JOYSTICK
} eSource_t;

typedef struct {
	eSource_t source;
	void* data;
} sEvent_t;

class EventManager : public Task {
public:
	static EventManager* getInstance();

	void registerObject(sEventType_t const*, MouseEventObject*);
	void unRegisterObject(sEventType_t const*);
	void registerObject(KeyboardEventObject*);
	void unRegisterObject(KeyboardEventObject*);

	void addEvent(sEvent_t& _event);

	void start();

protected:
	void taskRun();
	// private vars
private:
	static EventManager* mpInstance;

	std::queue<sMouseStatus_t*> mMouseEvQueue;
	std::map<sEventType_t const*, MouseEventObject*> mMouseEventLoc;
	std::map<sEventType_t const*, MouseEventObject*>::iterator mMouseEventIter;

	std::vector<KeyboardEventObject*> mKeyEventObjs;
	std::vector<KeyboardEventObject*>::iterator mKeyEventIter;

	Scheduler* pSched;
	volatile bool initialized = false;
	// private functions
private:
	explicit EventManager();
	explicit EventManager(EventManager&) {}
	~EventManager();
};