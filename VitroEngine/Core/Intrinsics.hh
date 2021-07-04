#pragma once

#if __clang__
	#define VT_COMPILER_CLANG __clang__
#elif _MSC_VER
	#define VT_COMPILER_MSVC _MSC_VER
#elif __GNUG__
	#define VT_COMPILER_GCC __GNUC__
#endif

#if VT_COMPILER_CLANG

	#define VT_DLLEXPORT __declspec(dllexport)
	#define VT_INLINE	 __attribute__((always_inline))

	#define vtDebugBreak()	__builtin_debugtrap()
	#define vtUnreachable() __builtin_unreachable()

#elif VT_COMPILER_MSVC

	#define VT_DLLEXPORT __declspec(dllexport)
	#define VT_INLINE	 __forceinline

	#define vtDebugBreak()	__debugbreak()
	#define vtUnreachable() __assume(0)

#elif VT_COMPILER_GCC

	#define VT_DLLEXPORT __attribute__((dllexport))
	#define VT_INLINE	 __attribute__((always_inline)) inline

	#include <csignal>
	#define vtDebugBreak()	std::raise(SIGTRAP)
	#define vtUnreachable() __builtin_unreachable()

#endif
