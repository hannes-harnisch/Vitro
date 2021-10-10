#pragma once

#if VT_SYSTEM_LINUX
	#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan/vulkan.h>

#if VT_SYSTEM_WINDOWS
typedef unsigned long				DWORD;
typedef const wchar_t*				LPCWSTR;
typedef void*						HANDLE;
typedef struct HINSTANCE__*			HINSTANCE;
typedef struct HWND__*				HWND;
typedef struct HMONITOR__*			HMONITOR;
typedef struct _SECURITY_ATTRIBUTES SECURITY_ATTRIBUTES;
	#include <vulkan/vulkan_win32.h>
#endif
