module;
#include "Trace/Assert.hh"

#include <csignal>
export module Vitro.Trace.SignalHandler;

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
