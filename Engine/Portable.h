#ifndef PORTABLE_H
#define PORTABLE_H

#define GLFW
//#define DEBUG

#ifdef GLFW
#include <GLFW\glfw3.h>
inline double getCurrentTime() {
	return glfwGetTime();
}

#define APP_CMD_CONFIG_CHANGED		0x0
#define APP_CMD_INIT_WINDOW			0x1
#define APP_CMD_DESTROY				0x2
#define APP_CMD_GAINED_FOCUS		0x3
#define APP_CMD_LOST_FOCUS			0x4
#define APP_CMD_LOW_MEMORY			0x5
#define APP_CMD_PAUSE				0x6
#define APP_CMD_RESUME				0x7
#define APP_CMD_SAVE_STATE			0x8
#define APP_CMD_START				0x9
#define APP_CMD_STOP				0xA
#define APP_CMD_TERM_WINDOW			0xB

#endif // GLFW

#endif // PORTABLE_H