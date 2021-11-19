module;
#include "VitroCore/Macros.hpp"

#include <string_view>
export module vt.Core.Process;

import vt.Core.VT_SYSTEM_MODULE.Process;

namespace vt
{
	using SystemProcess = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, Process);

	export class Process : public SystemProcess
	{
	public:
		Process(std::string_view cmdline_args) : SystemProcess(cmdline_args)
		{}

		void wait()
		{
			SystemProcess::wait();
		}

		auto native_handle()
		{
			return SystemProcess::native_handle();
		}
	};
}
