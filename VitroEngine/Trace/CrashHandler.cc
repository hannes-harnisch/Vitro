module;
#include <csignal>
#include <cstdio>
#include <format>
#include <string_view>
export module vt.Trace.CrashHandler;

import vt.VT_SYSTEM_MODULE.Utils;

namespace vt
{
	export [[noreturn]] void crash(std::string_view message)
	{
		auto text = std::format("Fatal error: {}", message);
		std::puts(text.data());

		VT_SYSTEM_NAME::show_error_message_box("Fatal error", message);
		std::exit(EXIT_FAILURE);
	}
}

extern "C"
{
	export void handle_arithmetic_signal(int)
	{
		vt::crash("Attempted to divide an integer by zero.");
	}

	export void handle_access_violation_signal(int)
	{
		vt::crash("Attempted to access a restricted memory location.");
	}

	export void handle_abort_signal(int)
	{
		vt::crash("A fatal error occurred that led to an abort signal.");
	}
}

namespace vt
{
	export void set_crash_handlers()
	{
		std::signal(SIGFPE, handle_arithmetic_signal);
		std::signal(SIGSEGV, handle_access_violation_signal);
		std::signal(SIGABRT, handle_abort_signal);
	}
}
