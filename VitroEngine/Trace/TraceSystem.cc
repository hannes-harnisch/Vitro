export module vt.Trace.TraceSystem;

import vt.Trace.CrashHandler;
import vt.Trace.Log;
import vt.VT_SYSTEM_MODULE.Trace;

namespace vt
{
	export class TraceSystem
	{
	public:
		TraceSystem()
		{
			set_crash_handlers();
			VT_SYSTEM_NAME::initialize_system_specific_tracing();
		}

	private:
		Logger logger;
	};
}
