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

	#define VT_DEBUG_BREAK()		__builtin_debugtrap()
	#define VT_ASSUME_UNREACHABLE() __builtin_unreachable()

#elif VT_COMPILER_MSVC

	#define VT_DLLEXPORT	 __declspec(dllexport)
	#define VT_ALWAYS_INLINE __forceinline

	#define VT_DEBUG_BREAK()		__debugbreak()
	#define VT_ASSUME_UNREACHABLE() __assume(0)

#elif VT_COMPILER_GCC

	#define VT_DLLEXPORT	 __attribute__((dllexport))
	#define VT_ALWAYS_INLINE __attribute__((always_inline)) inline

	#include <csignal>
	#define VT_DEBUG_BREAK()		std::raise(SIGTRAP)
	#define VT_ASSUME_UNREACHABLE() __builtin_unreachable()

#endif

#include <stdexcept>

#if VT_DEBUG

	#include <cstdlib>

	#define VT_UNREACHABLE()                                                                                                   \
		do                                                                                                                     \
		{                                                                                                                      \
			VT_DEBUG_BREAK();                                                                                                  \
			std::abort();                                                                                                      \
		} while(false)

	#define VT_ASSERT(CONDITION, MESSAGE)                                                                                      \
		do                                                                                                                     \
		{                                                                                                                      \
			if(!(CONDITION))                                                                                                   \
			{                                                                                                                  \
				VT_DEBUG_BREAK();                                                                                              \
				std::abort();                                                                                                  \
			}                                                                                                                  \
		} while(false)

	#define VT_ASSERT_PURE(CONDITION, MESSAGE) VT_ASSERT(CONDITION, MESSAGE)

	#define VT_ENSURE(CONDITION, MESSAGE)                                                                                      \
		do                                                                                                                     \
		{                                                                                                                      \
			if(!(CONDITION))                                                                                                   \
			{                                                                                                                  \
				VT_DEBUG_BREAK();                                                                                              \
				throw std::runtime_error(MESSAGE);                                                                             \
			}                                                                                                                  \
		} while(false)

#else

	#define VT_UNREACHABLE() VT_ASSUME_UNREACHABLE()

	#define VT_ASSERT_PURE(CONDITION, MESSAGE)
	#define VT_ASSERT(CONDITION, MESSAGE) static_cast<void>(CONDITION)

	#define VT_ENSURE(CONDITION, MESSAGE)                                                                                      \
		do                                                                                                                     \
		{                                                                                                                      \
			if(!(CONDITION))                                                                                                   \
				throw std::runtime_error(MESSAGE);                                                                             \
		} while(false)

#endif

// Use for checking result enum types from APIs where only negative values indicate failure.
#define VT_CHECK_RESULT(CONDITION, MESSAGE) VT_ENSURE((CONDITION) >= 0, MESSAGE)

#define VT_PASTE_IMPL(FIRST, SECOND) FIRST##SECOND
#define VT_PASTE(FIRST, SECOND)		 VT_PASTE_IMPL(FIRST, SECOND)

#define VT_STRINGIFY_IMPL(TOKEN) #TOKEN
#define VT_STRINGIFY(TOKEN)		 VT_STRINGIFY_IMPL(TOKEN)

#define VT_EXPAND_MACRO(MACRO) MACRO
#define VT_SYSTEM_HEADER	   <Core/VT_PASTE(Platform, VT_SYSTEM_MODULE)/##VT_EXPAND_MACRO(VT_SYSTEM_MODULE)##API.hpp>

#if VT_DYNAMIC_GPU_API

	#define VT_GPU_API_VARIANT_ARGS(OBJECT)                                                                                    \
		VT_GPU_API_NAME::VT_PASTE(VT_GPU_API_MODULE, OBJECT),                                                                  \
			VT_GPU_API_NAME_SECONDARY::VT_PASTE(VT_GPU_API_MODULE_SECONDARY, OBJECT)

#else

	#define VT_GPU_API_VARIANT_ARGS(OBJECT) VT_GPU_API_NAME::VT_PASTE(VT_GPU_API_MODULE, OBJECT)

#endif
