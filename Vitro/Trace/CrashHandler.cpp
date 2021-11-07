module;
#include <csignal>
#include <cstdio>
#include <exception>
#include <format>
#include <string_view>
export module vt.Trace.CrashHandler;

import vt.Core.Reflect;
import vt.Core.VT_SYSTEM_MODULE.Utils;

namespace vt
{
	export void on_failure(std::string_view message)
	{
		auto text = std::format("Fatal error: {}", message);
		std::puts(text.data());
		VT_SYSTEM_NAME::show_error_message_box("Fatal error", message);
	}

	export [[noreturn]] void crash(std::string_view message)
	{
		on_failure(message);
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
	void on_terminate()
	{
		std::string msg;

		auto exception = std::current_exception();
		if(!exception)
			msg = "The program was forced to terminate for unknown reasons.";

		try
		{
			std::rethrow_exception(exception);
		}
		catch(std::exception const& e)
		{
			msg = std::format("An instance of {} was thrown and not caught. Message:\n\n{}", name_of(e), e.what());
		}
		catch(...)
		{
			msg = "An unexpected exception was thrown.";
		}
		crash(msg);
	}

	export void set_crash_handlers()
	{
		std::set_terminate(on_terminate);

		std::signal(SIGFPE, handle_arithmetic_signal);
		std::signal(SIGSEGV, handle_access_violation_signal);
		std::signal(SIGABRT, handle_abort_signal);
	}
}
