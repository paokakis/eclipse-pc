#ifndef LOG_HPP
#define LOG_HPP
#include "Types.hpp"

class Log {
public:
	static void error(const char* pMessage, ...);
	static void warn(const char* pMessage, ...);
	static void info(const char* pMessage, ...);
	static void frames(const char* pMessage, ...);
};

#ifndef NDEBUG
#define packt_Log_debug(...) Log::debug(__VA_ARGS__)
#else
#define packt_Log_debug(...)
#endif // NDEBUG

#endif // LOG_HPP
