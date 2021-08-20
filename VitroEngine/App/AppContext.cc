module;
#include <atomic>
#include <vector>
export module Vitro.App.AppContext;

import Vitro.App.EventListener;
import Vitro.App.KeyCode;
import Vitro.App.Window;
import Vitro.App.WindowEvent;
import Vitro.VT_SYSTEM_MODULE.AppContext;

namespace vt
{
	export class AppContext : public VT_SYSTEM_NAME::AppContext, public EventListener
	{
		friend class AppSystem;

		std::atomic_bool&	 engineRunningStatus;
		std::vector<Window*> openWindows;

		AppContext(std::atomic_bool& engineRunningStatus) : engineRunningStatus(engineRunningStatus)
		{
			registerEventHandlers<&AppContext::onWindowOpen, &AppContext::onWindowClose, &AppContext::onEscapeHeld>();
		}

		void onWindowOpen(WindowOpenEvent& e)
		{
			openWindows.emplace_back(&e.window);
		}

		void onWindowClose(WindowCloseEvent& e)
		{
			std::erase(openWindows, &e.window);
			if(openWindows.empty())
				engineRunningStatus = false;
		}

		void onEscapeHeld(KeyDownEvent& e)
		{
			if(e.key == KeyCode::Escape && e.repeats == 10)
				e.window.close();
		}
	};
}
