#pragma once

#include "Core/Intrinsics.hh"

#include <string_view>

void crash(std::string_view const crashMessage);

#if VT_DEBUG

	#define vtEnsure(condition, message)                                                                                       \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				vtDebugBreak();                                                                                                \
		}

	#define vtAssertPure(condition, message)   vtEnsure(condition, message)
	#define vtAssert(condition, message)	   vtEnsure(condition, message)
	#define vtEnsureResult(condition, message) vtEnsure(static_cast<int>(condition) >= 0, message)
	#define vtAssertResult(condition, message) vtEnsure(static_cast<int>(condition) >= 0, message)

#else

	#define vtEnsure(condition, message)                                                                                       \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				crash("Assertion failed: " #condition);                                                                        \
		}

	#define vtAssertPure(condition, message)
	#define vtAssert(condition, message)	   static_cast<void>(condition)
	#define vtEnsureResult(condition, message) vtEnsure(static_cast<int>(condition) >= 0, message)
	#define vtAssertResult(condition, message) static_cast<void>(condition)

#endif
