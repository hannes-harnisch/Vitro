#pragma once

#include "Core/Intrinsics.hh"

#include <string_view>

void crash(const std::string_view crashMessage);

#if VE_DEBUG

	#define veEnsure(condition, message)                                                                                       \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				veDebugBreak();                                                                                                \
		}

	#define veAssert(condition, message)	   veEnsure(condition, message)
	#define veEnsureResult(condition, message) veEnsure(int(condition) >= 0, message)
	#define veAssertResult(condition, message) veEnsure(int(condition) >= 0, message)

#else

	#define veEnsure(condition, message)                                                                                       \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				crash("Assertion failed: " #condition);                                                                        \
		}

	#define veAssert(condition, message)	   void(condition)
	#define veEnsureResult(condition, message) veEnsure(int(condition) >= 0, message)
	#define veAssertResult(condition, message) void(condition)

#endif
