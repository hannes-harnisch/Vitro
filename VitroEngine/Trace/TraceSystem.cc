export module Vitro.Trace.TraceSystem;

import Vitro.Trace.Log;
import Vitro.Trace.SignalHandler;
import Vitro.VT_SYSTEM_MODULE.Trace;

namespace vt
{
	export class TraceSystem
	{
	public:
		TraceSystem()
		{
			setSignalHandlers();
			VT_SYSTEM_NAME::setSystemTracingState();
		}

	private:
		Logger logger;
	};
}
