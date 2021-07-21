module;
#include "Core/Intrinsics.hh"
export module Vitro.Trace.LogLevel;

namespace vt
{
	export enum class LogLevel : unsigned char {
		Verbose,
		Debug,
		Info,
		Warning,
		Error,
		Fatal,
	};

#define BGR_BLACK  ";40"
#define BGR_MAROON ";41"
#define BGR_OLIVE  ";43"
#define BGR_RED	   ";101"
#define TXT_SILVER "37"
#define TXT_LIME   "92"
#define TXT_AQUA   "96"
#define TXT_WHITE  "97"

	export char const* mapLogLevelToEscapeCodeParameters(LogLevel level)
	{
		switch(level)
		{
			case LogLevel::Verbose: return TXT_SILVER BGR_BLACK;
			case LogLevel::Debug: return TXT_AQUA BGR_BLACK;
			case LogLevel::Info: return TXT_LIME BGR_BLACK;
			case LogLevel::Warning: return TXT_WHITE BGR_OLIVE;
			case LogLevel::Error: return TXT_WHITE BGR_MAROON;
			case LogLevel::Fatal: return TXT_WHITE BGR_RED;
		}
		vtUnreachable();
	}
}
