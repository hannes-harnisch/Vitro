module;
#include <csignal>
#include <cstdio>
#include <format>
#include <string_view>
export module Vitro.Trace.CrashHandler;

import Vitro.VT_SYSTEM_MODULE.Utils;

namespace vt
{
	export [[noreturn]] void crash(std::string_view message)
	{
		auto text = std::format("Fatal error: {}", message);
		std::puts(text.data());

		VT_SYSTEM_NAME::showErrorMessageBox("Fatal error", message);
		std::exit(EXIT_FAILURE);
	}
}

extern "C"
{
	export void handleArithmeticSignal(int)
	{
		vt::crash("Attempted to divide an integer by zero.");
	}

	export void handleAccessViolationSignal(int)
	{
		vt::crash("Attempted to access a restricted memory location.");
	}

	export void handleAbortSignal(int)
	{
		vt::crash("A fatal error occurred that led to an abort signal.");
	}
}

namespace vt
{
	export void setCrashHandlers()
	{
		std::signal(SIGFPE, handleArithmeticSignal);
		std::signal(SIGSEGV, handleAccessViolationSignal);
		std::signal(SIGABRT, handleAbortSignal);
	}
}
