module;
#include <atomic>
#include <condition_variable>
#include <string_view>
#include <vector>
export module Vitro.Engine;

import Vitro.App.AppSystem;
import Vitro.Core.Tick;
import Vitro.Editor.Editor;
import Vitro.Graphics.GraphicsSystem;
import Vitro.Trace.Log;
import Vitro.Trace.TraceSystem;

namespace vt
{
	class Engine
	{
	public:
		Engine(std::vector<std::string_view> commandLineArgs) :
			commandLineArgs(std::move(commandLineArgs)), appSystem(isRunning)
		{}

		void run()
		{
			uint64_t previousTime = Tick::measureTime();
			while(isRunning)
			{
				tick.update(previousTime);

				update();
			}
		}

	private:
		std::atomic_bool isRunning = true;
		std::vector<std::string_view> commandLineArgs;
		Tick tick;
		TraceSystem traceSystem;
		AppSystem appSystem;
		GraphicsSystem graphicsSystem;
		Editor editor;

		void update()
		{
			appSystem.pollEvents();
		}
	};

	export void launchEngine(int argc, char* argv[])
	{
		static Engine engine({argv, argv + argc});
		engine.run();
	}
}
