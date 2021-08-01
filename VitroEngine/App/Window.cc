module;
#include <string_view>
export module Vitro.App.Window;

import Vitro.App.AppContextBase;
import Vitro.Core.Rectangle;
import Vitro.Graphics.GraphicsSystem;
import Vitro.VT_SYSTEM_MODULE.Window;

namespace vt
{
	export class Window final : public VT_SYSTEM_NAME::Window
	{
		using Base = VT_SYSTEM_NAME::Window;

	public:
		Window(std::string_view title, Rectangle rect = Base::DefaultRect) : Base(title, rect)
		{
			ensureCallIsOnMainThread();

			AppContextBase::get().notifyWindowMapping(handle(), *this);
			GraphicsSystem::get().notifyWindowConstruction(*this, handle());
		}

		Window(Window&& other) noexcept(false) : Base(std::move(other))
		{
			ensureCallIsOnMainThread();
			notifyMove(other);
		}

		~Window()
		{
			ensureCallIsOnMainThread();
			notifyDestruction();
		}

		Window& operator=(Window&& other) noexcept(false)
		{
			ensureCallIsOnMainThread();

			notifyDestruction();
			Base::operator=(std::move(other));
			notifyMove(other);
			return *this;
		}

	private:
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
