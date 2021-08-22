export module Vitro.Trace.TraceSystem;

import Vitro.Trace.CrashHandler;
import Vitro.Trace.Log;
import Vitro.VT_SYSTEM_MODULE.Trace;

namespace vt
{
	export class TraceSystem
	{
	public:
		TraceSystem()
		{
			setCrashHandlers();
			VT_SYSTEM_NAME::initializeSystemSpecificTracing();
		}

	private:
		Logger logger;
	};
}
