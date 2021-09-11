module;
#include "Core/Macros.hh"
export module vt.Trace.TraceSystem;

import vt.Trace.CrashHandler;
import vt.Trace.Log;
import vt.VT_SYSTEM_MODULE.TraceContext;

namespace vt
{
	using TraceContext = VT_SYSTEM_NAME::VT_PASTE(VT_SYSTEM_MODULE, TraceContext);

	export class TraceSystem
	{
	public:
		TraceSystem()
		{
			set_crash_handlers();
		}

	private:
		Logger		 logger;
		TraceContext trace_context;
	};
}
