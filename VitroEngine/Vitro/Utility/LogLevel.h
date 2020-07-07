#pragma once

namespace Vitro
{
	enum class LogLevel : uint8_t
	{
		Trace = 0x0F, // Renders with white text.
		Debug = 0x0B, // Renders with cyan text.
		Info  = 0x0A, // Renders with green text.
		Warn  = 0x6F, // Renders with white text and a dark yellow background.
		Error = 0x4F, // Renders with white text and a dark red background.
		Fatal = 0xCF  // Renders with white text and a red background.
	};

	std::string ToString(LogLevel level);
}

// These are used to set log level at buildtime
#define VTR_LOG_LEVEL_TRACE 0
#define VTR_LOG_LEVEL_DEBUG 1
#define VTR_LOG_LEVEL_INFO	2
#define VTR_LOG_LEVEL_WARN	3
#define VTR_LOG_LEVEL_ERROR 4
