module;
#include <atomic>
#include <condition_variable>
#include <stdexcept>
#include <string_view>
#include <vector>
export module Vitro.Engine;

import Vitro.App.AppSystem;
import Vitro.Core.Tick;
import Vitro.Editor.Editor;
import Vitro.Graphics.GraphicsSystem;
import Vitro.Trace.CrashHandler;
import Vitro.Trace.Log;
import Vitro.Trace.TraceSystem;

namespace vt
{
	class Engine
	{
	public:
		Engine(std::vector<std::string_view> commandLineArgs)
		try : commandLineArgs(std::move(commandLineArgs)), appSystem(isRunning)
		{}
		catch(std::exception const& e)
		{
			std::string msg = "An exception was thrown during launch:\n\n";
			crash(msg + e.what());
		}

		void run()
		{
			try
			{
				uint64_t previousTime = Tick::measureTime();
				while(isRunning)
				{
					tick.update(previousTime);
					update();
				}
			}
			catch(std::exception const& e)
			{
				std::string msg = "An exception was thrown while the engine was running:\n\n";
				crash(msg + e.what());
			}
		}

	private:
		std::atomic_bool			  isRunning = true;
		std::vector<std::string_view> commandLineArgs;
		Tick						  tick;
		TraceSystem					  traceSystem;
		AppSystem					  appSystem;
		GraphicsSystem				  graphicsSystem;
		Editor						  editor;

		void update()
		{
			appSystem.update();
		}
	};

	export void launchEngine(int argc, char* argv[])
	{
		static Engine engine({argv, argv + argc});
		engine.run();
	}
}
