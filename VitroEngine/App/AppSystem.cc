module;
#include <atomic>
#include <condition_variable>
#include <thread>
#include <vector>
export module Vitro.App.AppSystem;

import Vitro.App.AppContext;
import Vitro.App.Event;
import Vitro.App.EventBinding;
import Vitro.App.EventSystem;
import Vitro.App.KeyCode;
import Vitro.App.Input;
import Vitro.App.Window;
import Vitro.App.WindowEvent;

namespace vt
{
	export class AppSystem
	{
	public:
		AppSystem(std::atomic_bool& engineRunningStatus) :
			engineRunningStatus(engineRunningStatus),
			eventWorker(&EventSystem::runEventProcessing, &eventSystem),
			eventBinding(this, &AppSystem::onWindowOpen, &AppSystem::onWindowClose, &AppSystem::onEscapeHeld)
		{}

		~AppSystem()
		{
			eventSystem.quit();
		}

		void update()
		{
			appContext.pollEvents();
		}

	private:
		std::atomic_bool& engineRunningStatus;
		AppContext appContext;
		EventSystem eventSystem;
		Input input;
		std::jthread eventWorker;
		std::vector<Window*> openWindows;
		EventBinding eventBinding;

		bool onWindowOpen(WindowOpenEvent& e)
		{
			openWindows.emplace_back(&e.window);
			return true;
		}

		bool onWindowClose(WindowCloseEvent& e)
		{
			std::erase(openWindows, &e.window);
			if(openWindows.empty())
				engineRunningStatus = false;

			return true;
		}

		void onEscapeHeld(KeyDownEvent& e)
		{
			if(e.key == KeyCode::Escape && e.repeats == 10)
				e.window.close();
		}
	};
}
