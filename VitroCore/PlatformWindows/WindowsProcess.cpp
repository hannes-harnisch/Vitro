module;
#include "VitroCore/Macros.hpp"
#include "WindowsAPI.hpp"

#include <memory>
#include <string_view>
export module vt.Core.Windows.Process;

import vt.Core.Windows.Utils;

namespace vt::windows
{
	export class WindowsProcess
	{
	protected:
		WindowsProcess(std::string_view cmdline_args)
		{
			auto wide_args = widen_string(cmdline_args);

			STARTUPINFO start_info {
				.cb = sizeof start_info,
			};
			PROCESS_INFORMATION process_info;
			auto succeeded = ::CreateProcess(nullptr, wide_args.data(), nullptr, nullptr, false, 0, nullptr, nullptr,
											 &start_info, &process_info);
			process.reset(process_info.hProcess);
			thread.reset(process_info.hThread);
			check_winapi_error(succeeded, "Failed to create Windows process.");
		}

		void wait()
		{
			auto event = ::WaitForSingleObject(process.get(), INFINITE);
			check_winapi_error(event, "Failed to wait for Windows process.", WAIT_FAILED);
		}

		HANDLE native_handle()
		{
			return process.get();
		}

	private:
		struct HandleDeleter
		{
			using pointer = HANDLE;
			void operator()(HANDLE handle) const
			{
				auto succeeded = ::CloseHandle(handle);
				check_winapi_error(succeeded, "Failed to close handle.");
			}
		};
		using UniqueHandle = std::unique_ptr<HANDLE, HandleDeleter>;

		UniqueHandle process;
		UniqueHandle thread;
	};
}
