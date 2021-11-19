module;
#include <string_view>
export module vt.Trace.CrashHandler;

namespace vt
{
	export void				 on_failure(std::string_view message);
	export [[noreturn]] void crash(std::string_view message);
	export void				 set_crash_handlers();
}

extern "C"
{
	export void handle_arithmetic_signal(int);
	export void handle_access_violation_signal(int);
	export void handle_abort_signal(int);
}
