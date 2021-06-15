#pragma once

#include "Core/Intrinsics.hh"

#include <string_view>

void crash(std::string_view const crashMessage);

#if VT_DEBUG

	#define vtEnsure(condition, message)                                                                                       \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				veDebugBreak();                                                                                                \
		}

	#define vtAssert(condition, message)	   vtEnsure(condition, message)
	#define vtEnsureResult(condition, message) vtEnsure(int(condition) >= 0, message)
	#define vtAssertResult(condition, message) vtEnsure(int(condition) >= 0, message)

#else

	#define vtEnsure(condition, message)                                                                                       \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				crash("Assertion failed: " #condition);                                                                        \
		}

	#define vtAssert(condition, message)	   void(condition)
	#define vtEnsureResult(condition, message) vtEnsure(int(condition) >= 0, message)
	#define vtAssertResult(condition, message) void(condition)

#endif
