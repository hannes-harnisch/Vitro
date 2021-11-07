#pragma once

#if VT_SYSTEM_LINUX
	#define VK_USE_PLATFORM_XCB_KHR
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#if VT_SYSTEM_WINDOWS
// Forward-declare these instead of defining a use-platform macro so that vulkan.h doesn't include
// all of Windows.h just for these.

typedef unsigned long				DWORD;
typedef const wchar_t*				LPCWSTR;
typedef void*						HANDLE;
typedef struct HINSTANCE__*			HINSTANCE;
typedef struct HWND__*				HWND;
typedef struct HMONITOR__*			HMONITOR;
typedef struct _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;
	#include <vulkan/vulkan_win32.h>
#endif

#include <VulkanMemoryAllocator/VulkanMemoryAllocator/include/vk_mem_alloc.h>
