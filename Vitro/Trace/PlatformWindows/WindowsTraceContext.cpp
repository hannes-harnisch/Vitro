module;
#include "Core/Macros.hpp"
#include "Core/PlatformWindows/WindowsAPI.hpp"

#include <csignal>
export module vt.Trace.Windows.TraceContext;

import vt.Trace.CrashHandler;

namespace vt::windows
{
	export class WindowsTraceContext
	{
	public:
		WindowsTraceContext()
		{
			auto result = ::AddVectoredExceptionHandler(true, forward_to_standard_signal_handlers);
			VT_ASSERT(result, "Failed to set vectored exception handler.");

			auto std_out = ::GetStdHandle(STD_OUTPUT_HANDLE);
			VT_ASSERT(std_out != INVALID_HANDLE_VALUE, "Failed to get standard output handle.");

			DWORD mode;
			BOOL  succeeded = ::GetConsoleMode(std_out, &mode);
			VT_ASSERT(succeeded, "Failed to query current console mode.");

			mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			succeeded = ::SetConsoleMode(std_out, mode);
			VT_ASSERT(succeeded, "Failed to enable virtual terminal processing.");
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
