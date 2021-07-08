module;
#include <thread>
export module Vitro.Trace.TraceSystem;

import Vitro.Trace.Log;
import Vitro.Trace.SignalHandler;
import Vitro.VT_SYSTEM_MODULE.Trace;

namespace vt
{
	export class TraceSystem
	{
	public:
		TraceSystem() : logWorker(&Logger::runLogProcessing, &logger)
		{
			setSignalHandlers();
			VT_SYSTEM_NAME::setSystemTracingState();
		}

		~TraceSystem()
		{
			logger.quit();
		}

	private:
		Logger logger;
		std::jthread logWorker;
	};
}
