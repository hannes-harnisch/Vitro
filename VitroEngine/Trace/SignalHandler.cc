module;
#include <csignal>
#include <cstdio>
#include <string_view>
export module Vitro.Trace.SignalHandler;

namespace vt
{
	void crash(std::string_view crashMessage)
	{
		std::printf("Critical failure: %s", crashMessage.data());
		std::exit(EXIT_FAILURE);
	}
}

extern "C"
{
	export void handleArithmeticException(int)
	{
		vt::crash("Fatal arithmetic exception.");
	}

	export void handleAccessViolation(int)
	{
		vt::crash("Illegal memory access.");
	}

	export void handleAbnormalExit(int)
	{
		vt::crash("Abnormal exit induced.");
	}
}

namespace vt
{
	export void setSignalHandlers()
	{
		std::signal(SIGFPE, handleArithmeticException);
		std::signal(SIGSEGV, handleAccessViolation);
		std::signal(SIGABRT, handleAbnormalExit);
	}
}
