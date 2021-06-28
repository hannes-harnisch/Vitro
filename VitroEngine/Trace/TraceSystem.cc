module;
#include <thread>
export module Vitro.Trace.TraceSystem;

import Vitro.Trace.Log;
import Vitro.Trace.SignalHandler;
import Vitro.VT_SYSTEM_MODULE.SignalHandler;

namespace vt
{
	export class TraceSystem
	{
	public:
		TraceSystem() : logWorker(&Logger::runQueueProcessing, &logger)
		{
			setCommonSignalHandlers();
			VT_SYSTEM_NAME::setPlatformSignalHandlers();
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
