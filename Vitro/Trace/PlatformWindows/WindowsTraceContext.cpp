module;
#include "VitroCore/Macros.hpp"
#include "VitroCore/PlatformWindows/WindowsAPI.hpp"

#include <csignal>
export module vt.Trace.Windows.TraceContext;

import vt.Core.Windows.Utils;
import vt.Trace.CrashHandler;

namespace vt::windows
{
	export class WindowsTraceContext
	{
	public:
		WindowsTraceContext()
		{
			set_exception_handler();
			enable_ansi_color_sequences();
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

		static void set_exception_handler()
		{
			auto handle = ::AddVectoredExceptionHandler(true, forward_to_standard_signal_handlers);
			VT_ENSURE(handle, "Failed to set vectored exception handler.");
		}

		static void enable_ansi_color_sequences()
		{
			auto std_out = ::GetStdHandle(STD_OUTPUT_HANDLE);
			check_winapi_error(std_out, "Failed to get standard output handle.", INVALID_HANDLE_VALUE);

			if(!std_out)
				return; // Will be null if there is no console attached, such as in release mode, so just return.

			DWORD mode;
			auto  succeeded = ::GetConsoleMode(std_out, &mode);
			check_winapi_error(succeeded, "Failed to query current console mode.");

			mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			succeeded = ::SetConsoleMode(std_out, mode);
			check_winapi_error(succeeded, "Failed to enable virtual terminal processing.");
		}
	};
}
