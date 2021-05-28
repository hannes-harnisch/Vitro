module;
#include "Windows.API.hh"

#include <csignal>
export module Vitro.Windows.SignalHandler;

import Vitro.Trace.SignalHandler;

namespace Windows
{
	LONG NTAPI forwardToStandardSignalHandlers(EXCEPTION_POINTERS* const exceptionInfo)
	{
		switch(exceptionInfo->ExceptionRecord->ExceptionCode)
		{
			case STATUS_ACCESS_VIOLATION: handleAccessViolation(SIGSEGV); break;
			case STATUS_INTEGER_DIVIDE_BY_ZERO: handleArithmeticException(SIGFPE); break;
			case STATUS_STACK_OVERFLOW: handleAccessViolation(SIGSEGV); break;
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}

	export void setPlatformSignalHandlers()
	{
		::AddVectoredExceptionHandler(true, forwardToStandardSignalHandlers);
	}
}
