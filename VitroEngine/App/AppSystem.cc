module;
#include <atomic>
export module Vitro.App.AppSystem;

import Vitro.App.AppContext;
import Vitro.App.EventSystem;
import Vitro.App.Input;

namespace vt
{
	export class AppSystem
	{
	public:
		AppSystem(std::atomic_bool& engineRunningStatus) : appContext(engineRunningStatus)
		{}

		void update()
		{
			appContext.pollEvents();
		}

	private:
		EventSystem eventSystem;
		AppContext	appContext;
		Input		input;
	};
}
