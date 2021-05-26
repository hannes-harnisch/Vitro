module;
#include "Trace/Assert.hh"

#include <csignal>
export module Vitro.Trace.SignalHandler;

extern "C"
{
	export void handleArithmeticException(int)
	{
		crash("Fatal arithmetic exception.");
	}

	export void handleAccessViolation(int)
	{
		crash("Illegal memory access.");
	}

	export void handleAbnormalExit(int)
	{
		crash("Abnormal exit induced.");
	}
}

export void setCommonSignalHandlers()
{
	std::signal(SIGFPE, handleArithmeticException);
	std::signal(SIGSEGV, handleAccessViolation);
	std::signal(SIGABRT, handleAbnormalExit);
}
