#ifndef TIMEMANAGER_HPP
#define TIMEMANAGER_HPP

#include "Tools/Types.hpp"
#include <ctime>

class TimeManager 
{
public:
	static TimeManager* getInstance();
	void reset();
	void update();
	double now();
	float elapsed() { return mElapsed; };
	float elapsedTotal() { return mElapsedTotal; };
private:
	TimeManager();
	TimeManager(TimeManager&) {}

	static TimeManager* mp_instance;
	double mFirstTime;
	double mLastTime;
	float mElapsed;
	float mElapsedTotal;
};

#endif