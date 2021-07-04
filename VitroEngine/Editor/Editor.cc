module;
#include <new>
export module Vitro.Editor.Editor;

import Vitro.App.EventBinding;
import Vitro.App.KeyCode;
import Vitro.App.Window;
import Vitro.App.WindowEvent;

namespace vt
{
	export class Editor
	{
	public:
		Editor() : window(VT_ENGINE_NAME)
		{
			window.open();
		}

	private:
		Window window;
	};
}
