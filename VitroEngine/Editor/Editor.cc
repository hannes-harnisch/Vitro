export module Vitro.Editor.Editor;

import Vitro.App.Window;

export class Editor
{
public:
	Editor() : window(VE_ENGINE_NAME)
	{
		window.open();
	}

private:
	Window window;
};
