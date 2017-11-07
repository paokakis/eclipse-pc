#include <Windows.h>
#include "Log.hpp"
#include <cstdarg>
#include <stdio.h>
#include <time.h>
#include <memory.h>
#include "../Graphics/GraphicsManager.hpp"

char time_buf[100];
char buf[500];

static void logStr(const char* fmt, va_list args1, va_list args2)
{
	struct timespec ts;

	memset(time_buf, 0, 100);
	memset(buf, 0, 500);

	timespec_get(&ts, TIME_UTC);
	size_t rc = strftime(time_buf, sizeof time_buf, "%D %T", gmtime(&ts.tv_sec));
	snprintf(time_buf + rc, sizeof time_buf - rc, ".%06ld UTC", ts.tv_nsec / 1000);

	const int tmp = vsnprintf(NULL, 0, fmt, args1);
	vsnprintf(buf, sizeof buf, fmt, args2);
}

void Log::info(const char* fmt, ...)
{
#ifdef _DEBUG
	va_list args1;
	va_start(args1, fmt);
	va_list args2;
	va_copy(args2, args1);
	logStr(fmt, args1, args2);
	va_end(args1);
	va_end(args2);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 10);
	printf("%s [INFO]: %s\n", time_buf, buf);
	SetConsoleTextAttribute(hConsole, 7);
#endif
}

void Log::warn(const char* fmt, ...)
{
#ifdef _DEBUG
	va_list args1;
	va_start(args1, fmt);
	va_list args2;
	va_copy(args2, args1);
	logStr(fmt, args1, args2);
	va_end(args1);
	va_end(args2);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 14);
	printf("%s [WARNING]: %s\n", time_buf, buf);
	SetConsoleTextAttribute(hConsole, 7);
#endif
}

void Log::error(const char* fmt, ...)
{
#ifdef _DEBUG
	va_list args1;
	va_start(args1, fmt);
	va_list args2;
	va_copy(args2, args1);
	logStr(fmt, args1, args2);
	va_end(args1);
	va_end(args2);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 12);
	printf("%s [ERROR]: %s\n", time_buf, buf);
	SetConsoleTextAttribute(hConsole, 7);
	getchar();
#endif
}

void Log::frames(const char* fmt, ...)
{
#ifdef _DEBUG
	va_list args1;
	va_start(args1, fmt);
	va_list args2;
	va_copy(args2, args1);
	logStr(fmt, args1, args2);
	va_end(args1);
	va_end(args2);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 12);
	printf("%s [frames/sec]: %s\r", time_buf, buf);
	SetConsoleTextAttribute(hConsole, 7);
#endif
}