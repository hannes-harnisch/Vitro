#pragma once

#if __clang__
	#define VE_COMPILER_CLANG __clang__
#elif _MSC_VER
	#define VE_COMPILER_MSVC _MSC_VER
#elif __GNUG__
	#define VE_COMPILER_GCC __GNUC__
#endif

#if VE_COMPILER_CLANG

	#define VE_DLLEXPORT __declspec(dllexport)
	#define VE_INLINE	 __attribute__((always_inline))

	#define veDebugBreak() __builtin_debugtrap()

#elif VE_COMPILER_MSVC

	#define VE_DLLEXPORT __declspec(dllexport)
	#define VE_INLINE	 __forceinline

	#define veDebugBreak() __debugbreak()

#elif VE_COMPILER_GCC

	#define VE_DLLEXPORT __attribute__((dllexport))
	#define VE_INLINE	 __attribute__((always_inline)) inline

	#include <csignal>
	#define veDebugBreak() std::raise(SIGTRAP)

#endif
