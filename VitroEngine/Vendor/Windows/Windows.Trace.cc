module;
#include "Windows.API.hh"

#include <csignal>
export module Vitro.Windows.Trace;

import Vitro.Trace.CrashHandler;

namespace vt::windows
{
	LONG NTAPI forwardToStandardSignalHandlers(EXCEPTION_POINTERS* exceptionInfo)
	{
		switch(exceptionInfo->ExceptionRecord->ExceptionCode)
		{
			case STATUS_ACCESS_VIOLATION: handleAccessViolationSignal(SIGSEGV); break;
			case STATUS_INTEGER_DIVIDE_BY_ZERO: handleArithmeticSignal(SIGFPE); break;
			case STATUS_STACK_OVERFLOW: handleAccessViolationSignal(SIGSEGV); break;
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}

	export void initializeSystemSpecificTracing()
	{
		::AddVectoredExceptionHandler(true, forwardToStandardSignalHandlers);

		auto stdOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
		::SetConsoleMode(stdOut, ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_VIRTUAL_TERMINAL_INPUT);
	}
}
