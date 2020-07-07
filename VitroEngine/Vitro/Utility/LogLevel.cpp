#include "_pch.h"
#include "LogLevel.h"

#define VTR_STRING_CONVERSION_CASE(X)                                                                                  \
	case LogLevel::X: return #X

namespace Vitro
{
	std::string ToString(LogLevel level)
	{
		switch(level)
		{
			VTR_STRING_CONVERSION_CASE(Trace);
			VTR_STRING_CONVERSION_CASE(Debug);
			VTR_STRING_CONVERSION_CASE(Info);
			VTR_STRING_CONVERSION_CASE(Warn);
			VTR_STRING_CONVERSION_CASE(Error);
			VTR_STRING_CONVERSION_CASE(Fatal);
		}
		return "Unknown log level: " + std::to_string(static_cast<int>(level));
	}
}
