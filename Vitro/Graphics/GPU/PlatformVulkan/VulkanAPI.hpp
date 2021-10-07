#pragma once

#if VT_SYSTEM_MODULE == Windows
	#define VK_USE_PLATFORM_WIN32_KHR
#elif VT_SYSTEM_MODULE == Linux
	#define VK_USE_PLATFORM_XCB_KHR
#endif

#include <vulkan/vulkan.h>
