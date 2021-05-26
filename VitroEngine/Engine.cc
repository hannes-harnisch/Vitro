module;
#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <string_view>
#include <vector>
export module Vitro.Engine;

import Vitro.App.AppSystem;
import Vitro.Core.Singleton;
import Vitro.Core.Tick;
import Vitro.Editor.Editor;
import Vitro.Graphics.GraphicsSystem;
import Vitro.Math.Matrix; //TODO
import Vitro.Task.TaskSystem;
import Vitro.Trace.Log;
import Vitro.Trace.TraceSystem;

class Engine
{
public:
	Engine(std::vector<std::string_view> commandLineArgs) : commandLineArgs(std::move(commandLineArgs)), appSystem(isRunning)
	{}

	int run()
	{
		uint64_t previousTime = Tick::measureTime();
		while(isRunning)
		{
			tick.update(previousTime);

			update();
		}
		return EXIT_SUCCESS;
	}

private:
	std::atomic_bool isRunning = true;
	std::vector<std::string_view> commandLineArgs;
	Tick tick;
	TraceSystem traceSystem;
	TaskSystem taskSystem;
	AppSystem appSystem;
	GraphicsSystem graphicsSystem;
	Editor editor;

	void update()
	{
		Float3x3 m {1, -3, -4, 2, 1, 0, 3, -2, 5};
		Log().info(determinant(m));
		appSystem.pollEvents();
	}
};

export int engineMain(int argc, char* argv[])
{
	static Engine engine({argv, argv + argc});
	return engine.run();
}
