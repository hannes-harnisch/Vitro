module;
#include <atomic>
#include <condition_variable>
#include <cstdlib>
#include <stdexcept>
#include <string_view>
#include <vector>
export module vt.Engine;

import vt.App.AppSystem;
import vt.App.EventSystem;
import vt.Core.Tick;
import vt.Editor.Editor;
import vt.Graphics.GraphicsSystem;
import vt.Trace.CrashHandler;
import vt.Trace.Log;
import vt.Trace.TraceSystem;

namespace vt
{
	class Engine
	{
	public:
		Engine(std::vector<std::string_view> command_line_args)
		try : command_line_args(std::move(command_line_args)), app_system(is_running)
		{}
		catch(std::exception const& e)
		{
			std::string msg = "An exception was thrown during launch:\n\n";
			crash(msg + e.what());
		}

		int run()
		{
			try
			{
				uint64_t previous_time = Tick::measure_time();
				while(is_running)
				{
					tick.update(previous_time);
					update();
				}
				return EXIT_SUCCESS;
			}
			catch(std::exception const& e)
			{
				std::string msg = "An exception was thrown while the engine was running:\n\n";
				on_failure(msg + e.what());
				return EXIT_FAILURE;
			}
		}

	private:
		std::atomic_bool			  is_running = true;
		std::vector<std::string_view> command_line_args;
		Tick						  tick;
		TraceSystem					  trace_system;
		EventSystem					  event_system;
		AppSystem					  app_system;
		GraphicsSystem				  graphics_system;
		Editor						  editor;

		void update()
		{
			app_system.update();
		}
	};
}

export int main(int argc, char* argv[])
{
	static vt::Engine engine({argv, argv + argc});
	return engine.run();
}
