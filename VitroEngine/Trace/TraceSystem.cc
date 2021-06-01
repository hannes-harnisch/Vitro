module;
#include <thread>
export module Vitro.Trace.TraceSystem;

import Vitro.Trace.Log;
import Vitro.Trace.SignalHandler;
import Vitro.VE_SYSTEM.SignalHandler;

export class TraceSystem
{
public:
	TraceSystem() : logWorker(&Logger::runQueueProcessing, &logger)
	{
		setCommonSignalHandlers();
		VE_SYSTEM::setPlatformSignalHandlers();
	}

	~TraceSystem()
	{
		logger.quit();
	}

private:
	Logger logger;
	std::jthread logWorker;
};
