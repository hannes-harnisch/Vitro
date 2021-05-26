#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_SMART_HANDLE
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1

#if VE_SYSTEM == Windows
	#define VK_USE_PLATFORM_WIN32_KHR
#else
	#error System unspecified.
#endif

#include <vulkan/vulkan.hpp>
