module;
#include <string_view>
export module Vitro.App.Window;

import Vitro.App.AppContextBase;
import Vitro.Graphics.GraphicsSystem;
import Vitro.Math.Rectangle;
import Vitro.VT_SYSTEM_MODULE.Window;

namespace vt
{
	export class Window final : public VT_SYSTEM_NAME::Window
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
			ensureCallIsOnMainThread();

			AppContextBase::get().notifyWindowMapping(getHandle(), *this);
			GraphicsSystem::get().notifyWindowConstruction(*this, getHandle());
		}

		Window(Window&& other) noexcept : Platform(std::move(other))
		{
			ensureCallIsOnMainThread();
			notifyMove(other);
		}

		~Window()
		{
			ensureCallIsOnMainThread();
			notifyDestruction();
		}

		Window& operator=(Window&& other) noexcept
		{
			ensureCallIsOnMainThread();

			notifyDestruction();
			Platform::operator=(std::move(other));
			notifyMove(other);
			return *this;
		}

	private:
		void notifyDestruction()
		{
			AppContextBase::get().notifyWindowDestruction(getHandle());
			GraphicsSystem::get().notifyWindowDestruction(*this);
		}

		void notifyMove(Window& oldWindow)
		{
			AppContextBase::get().notifyWindowMapping(getHandle(), *this);
			GraphicsSystem::get().notifyWindowReplacement(oldWindow, *this);
		}
	};
}
