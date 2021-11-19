module;
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
		WindowsProcess(std::string_view cmdline_args);

		void   wait();
		HANDLE native_handle();

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
