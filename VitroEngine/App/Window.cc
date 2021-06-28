module;
#include <string_view>
export module Vitro.App.Window;

import Vitro.App.AppContextBase;
import Vitro.Graphics.GraphicsSystem;
import Vitro.Math.Rectangle;
import Vitro.VT_SYSTEM_MODULE.Window;

namespace vt
{
	export class Window : public VT_SYSTEM_NAME::Window
	{
		using Platform = VT_SYSTEM_NAME::Window;

	public:
		// TODO ICE
		Window(std::string_view const title,
			   Rectangle const size = Platform::DefaultSize,
			   int const x			= Platform::DefaultX,
			   int const y			= Platform::DefaultY) :
			Platform(title, size, {x, y})
		{
			AppContextBase::get().notifyWindowConstruction(handle(), *this);
			GraphicsSystem::get().notifyWindowConstruction(handle(), *this);
		}

		Window(Window&& other) noexcept : Platform(std::move(other))
		{
			notifyReplacement(other);
		}

		~Window()
		{
			notifyDestruction();
		}

		Window& operator=(Window&& other) noexcept
		{
			Platform::operator=(std::move(other));
			notifyDestruction();
			notifyReplacement(other);
			return *this;
		}

	private:
		void notifyReplacement(Window& oldWindow)
		{
			AppContextBase::get().notifyWindowConstruction(oldWindow.handle(), *this);
			GraphicsSystem::get().notifyWindowReplacement(oldWindow, *this);
		}

		void notifyDestruction()
		{
			AppContextBase::get().notifyWindowDestruction(handle());
			GraphicsSystem::get().notifyWindowDestruction(*this);
		}
	};
}
