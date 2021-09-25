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

#include <format>
#include <stdexcept>

#if VT_DEBUG

	#include <cstdlib>

	#define VT_UNREACHABLE()                                                                                                   \
		{                                                                                                                      \
			VT_DEBUG_BREAK();                                                                                                  \
			std::abort();                                                                                                      \
		}

	#define VT_ASSERT(condition, message)                                                                                      \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
			{                                                                                                                  \
				VT_DEBUG_BREAK();                                                                                              \
				std::abort();                                                                                                  \
			}                                                                                                                  \
		}

	#define VT_ASSERT_PURE(condition, message)	 VT_ASSERT(condition, message)
	#define VT_ASSERT_RESULT(condition, message) VT_ASSERT((condition) >= 0, message)

	#define VT_ENSURE(condition, message, ...)                                                                                 \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
			{                                                                                                                  \
				VT_DEBUG_BREAK();                                                                                              \
				throw std::runtime_error(std::format(message, __VA_ARGS__));                                                   \
			}                                                                                                                  \
		}

	#define VT_ENSURE_RESULT(condition, message) VT_ENSURE((condition) >= 0, message)

#else

	#define VT_UNREACHABLE() VT_ASSUME_UNREACHABLE()

	#define VT_ASSERT_PURE(condition, message)
	#define VT_ASSERT(condition, message)		 static_cast<void>(condition)
	#define VT_ASSERT_RESULT(condition, message) static_cast<void>(condition)

	#define VT_ENSURE(condition, message, ...)                                                                                 \
		{                                                                                                                      \
			if(!(condition))                                                                                                   \
				throw std::runtime_error(std::format(message, __VA_ARGS__));                                                   \
		}

	#define VT_ENSURE_RESULT(condition, message) VT_ENSURE((condition) >= 0, message)

#endif

#define VT_PASTE_IMPL(first, second) first##second
#define VT_PASTE(first, second)		 VT_PASTE_IMPL(first, second)

#define VT_STRINGIFY_IMPL(token) #token
#define VT_STRINGIFY(token)		 VT_STRINGIFY_IMPL(token)

#define VT_EXPAND_MACRO(macro) macro
#define VT_SYSTEM_HEADER	   <Core/VT_PASTE(Platform, VT_SYSTEM_MODULE)/##VT_EXPAND_MACRO(VT_SYSTEM_MODULE)##API.hpp>

#if VT_DYNAMIC_GPU_API

	#define VT_GPU_API_VARIANT_ARGS(object)                                                                                    \
		VT_GPU_API_NAME::VT_PASTE(VT_GPU_API_MODULE, object),                                                                  \
			VT_GPU_API_NAME_SECONDARY::VT_PASTE(VT_GPU_API_MODULE_SECONDARY, object)

#else

	#define VT_GPU_API_VARIANT_ARGS(object) VT_GPU_API_NAME::VT_PASTE(VT_GPU_API_MODULE, object)

#endif
