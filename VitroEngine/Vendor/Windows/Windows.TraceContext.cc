module;
#include "Windows.API.hh"

#include <csignal>
export module vt.Windows.TraceContext;

import vt.Trace.CrashHandler;
import vt.Trace.Log;

namespace vt::windows
{
	export class WindowsTraceContext
	{
	public:
		WindowsTraceContext()
		{
			::AddVectoredExceptionHandler(true, forward_to_standard_signal_handlers);

			auto std_out = ::GetStdHandle(STD_OUTPUT_HANDLE);
			::SetConsoleMode(std_out, ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_VIRTUAL_TERMINAL_INPUT);
		}

	private:
		static LONG NTAPI forward_to_standard_signal_handlers(EXCEPTION_POINTERS* exception_info)
		{
			switch(exception_info->ExceptionRecord->ExceptionCode)
			{
				case STATUS_ACCESS_VIOLATION:
				case STATUS_STACK_OVERFLOW: handle_access_violation_signal(SIGSEGV); break;
				case STATUS_INTEGER_DIVIDE_BY_ZERO: handle_arithmetic_signal(SIGFPE); break;
			}
			return EXCEPTION_CONTINUE_SEARCH;
		}
	};
}
