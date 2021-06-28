import Vitro.App.App;
import Vitro.App.Window;

class VitroTests : public vt::App
{
	vt::Window window;

public:
	VitroTests() : window {"A", {600, 600}, 600, 600}
	{
		window.open();
	}
};
