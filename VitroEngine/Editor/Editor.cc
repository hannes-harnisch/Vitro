module;
#include <new>
export module vt.Editor.Editor;

import vt.App.KeyCode;
import vt.App.Window;
import vt.App.WindowEvent;

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
