#pragma once

#if __clang__
	#define VT_COMPILER_CLANG __clang__
#elif _MSC_VER
	#define VT_COMPILER_MSVC _MSC_VER
#elif __GNUG__
	#define VT_COMPILER_GCC __GNUC__
#endif

#if VT_COMPILER_CLANG

	#define VT_DLLEXPORT	 __declspec(dllexport)
	#define VT_ALWAYS_INLINE __attribute__((always_inline))

	#define vtDebugBreak()		  __builtin_debugtrap()
	#define vtAssumeUnreachable() __builtin_unreachable()

#elif VT_COMPILER_MSVC

	#define VT_DLLEXPORT	 __declspec(dllexport)
	#define VT_ALWAYS_INLINE __forceinline

	#define vtDebugBreak()		  __debugbreak()
	#define vtAssumeUnreachable() __assume(0)

#elif VT_COMPILER_GCC

	#define VT_DLLEXPORT	 __attribute__((dllexport))
	#define VT_ALWAYS_INLINE __attribute__((always_inline)) inline

	#include <csignal>
	#define vtDebugBreak()		  std::raise(SIGTRAP)
	#define vtAssumeUnreachable() __builtin_unreachable()

#endif

#include <format>
#include <stdexcept>

#if VT_DEBUG

	#include <cstdlib>

	#define vtUnreachable()                                                                                                    \
		{                                                                                                                      \
			vtDebugBreak();                                                                                                    \
			vtAssumeUnreachable();                                                                                             \
		}

	#define vtAssert(condition, message)                                                                                       \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
			{                                                                                                                  \
				vtDebugBreak();                                                                                                \
				std::exit(EXIT_FAILURE);                                                                                       \
			}                                                                                                                  \
		}

	#define vtAssertPure(condition, message)   vtAssert(condition, message)
	#define vtAssertResult(condition, message) vtAssert(static_cast<int>(condition) >= 0, message)

	#define vtEnsure(condition, message, ...)                                                                                  \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
			{                                                                                                                  \
				vtDebugBreak();                                                                                                \
				throw std::runtime_error(std::format(message, __VA_ARGS__));                                                   \
			}                                                                                                                  \
		}

	#define vtEnsureResult(condition, message) vtEnsure(static_cast<int>(condition) >= 0, message)

#else

	#define vtUnreachable() vtAssumeUnreachable()

	#define vtAssertPure(condition, message)
	#define vtAssert(condition, message)	   static_cast<void>(condition)
	#define vtAssertResult(condition, message) static_cast<void>(condition)

	#define vtEnsure(condition, message, ...)                                                                                  \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				throw std::runtime_error(std::format(message, __VA_ARGS__));                                                   \
		}

	#define vtEnsureResult(condition, message) vtEnsure(static_cast<int>(condition) >= 0, message)

#endif
