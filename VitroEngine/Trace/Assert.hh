#pragma once

#include "Core/Intrinsics.hh"

#include <stdexcept>

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
				throw std::runtime_error("Assertion failed: " #condition);                                                     \
		}

	#define vtAssertPure(condition, message)
	#define vtAssert(condition, message)	   static_cast<void>(condition)
	#define vtEnsureResult(condition, message) vtEnsure(static_cast<int>(condition) >= 0, message)
	#define vtAssertResult(condition, message) static_cast<void>(condition)

#endif
