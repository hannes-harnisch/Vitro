export module Vitro.Editor.Editor;

import Vitro.App.Window;

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
