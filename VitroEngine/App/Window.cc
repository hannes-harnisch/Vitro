module;
#include "Trace/Assert.hh"

#include <string_view>
#include <thread>
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
			ensureCallIsOnMainThread();
			AppContextBase::get().notifyWindowMapping(handle(), *this);
			GraphicsSystem::get().notifyWindowConstruction(*this, handle());
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
		void ensureCallIsOnMainThread()
		{
			bool const isOnMainThread = AppContextBase::get().mainThreadId() == std::this_thread::get_id();
			vtEnsure(isOnMainThread, "Window operations must happen on the main thread.");
		}

		void notifyDestruction()
		{
			AppContextBase::get().notifyWindowDestruction(handle());
			GraphicsSystem::get().notifyWindowDestruction(*this);
		}

		void notifyMove(Window& oldWindow)
		{
			AppContextBase::get().notifyWindowMapping(handle(), *this);
			GraphicsSystem::get().notifyWindowReplacement(oldWindow, *this);
		}
	};
}
