module;
#include <string_view>
export module Vitro.App.Window;

import Vitro.App.AppContextBase;
import Vitro.VE_SYSTEM.Window;

export class Window : public VE_SYSTEM::Window
{
	using Platform = VE_SYSTEM::Window;

public:
	// TODO: change to Int2 after compiler fix
	Window(std::string_view title,
		   Rectangle size = Platform::DefaultSize,
		   int x		  = Platform::DefaultX,
		   int y		  = Platform::DefaultY) :
		Platform(title, size, {x, y})
	{
		AppContextBase::get().submitWindow(handle(), this);
	}

	Window(Window&& other) noexcept : Platform(std::move(other))
	{
		replaceOtherInWindowMap(other);
	}

	~Window()
	{
		eraseSelfFromWindowMap();
	}

	Window& operator=(Window&& other) noexcept
	{
		Platform::operator=(std::move(other));
		eraseSelfFromWindowMap();
		replaceOtherInWindowMap(other);
		return *this;
	}

private:
	void replaceOtherInWindowMap(WindowBase& other)
	{
		AppContextBase::get().submitWindow(other.handle(), this);
	}

	void eraseSelfFromWindowMap()
	{
		AppContextBase::get().removeWindow(handle());
	}
};
