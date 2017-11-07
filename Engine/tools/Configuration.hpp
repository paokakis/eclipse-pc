#ifndef _PACKT_CONFIGURATION_HPP_
#define _PACKT_CONFIGURATION_HPP_
#include "Types.hpp"

typedef int32_t screen_rot;
const screen_rot ROTATION_0 = 0;
const screen_rot ROTATION_90 = 1;
const screen_rot ROTATION_180 = 2;
const screen_rot ROTATION_270 = 3;

class Configuration {
public:
	Configuration();
	screen_rot getRotation() { return mRotation; };
private:
	//void findRotation(JNIEnv* pEnv);
	screen_rot mRotation;
};
#endif