module;
#include "VitroCore/Macros.hpp"
#include "WindowsAPI.hpp"

#include <memory>
#include <string_view>
module vt.Core.Windows.Process;

import vt.Core.Windows.Utils;

namespace vt::windows
{
	WindowsProcess::WindowsProcess(std::string_view cmdline_args)
	{
		auto wide_args = widen_string(cmdline_args);

		STARTUPINFO start_info {
			.cb = sizeof start_info,
		};
		PROCESS_INFORMATION process_info;
		auto succeeded = ::CreateProcess(nullptr, wide_args.data(), nullptr, nullptr, false, 0, nullptr, nullptr, &start_info,
										 &process_info);
		process.reset(process_info.hProcess);
		thread.reset(process_info.hThread);
		check_winapi_error(succeeded, "Failed to create Windows process.");
	}

	void WindowsProcess::wait()
	{
		auto event = ::WaitForSingleObject(process.get(), INFINITE);
		check_winapi_error(event, "Failed to wait for Windows process.", WAIT_FAILED);
	}

	HANDLE WindowsProcess::native_handle()
	{
		return process.get();
	}
}
