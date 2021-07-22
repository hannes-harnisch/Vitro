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
		using Base = VT_SYSTEM_NAME::Window;

	public:
		Window(std::string_view title, Rectangle rect = Base::DefaultRect) : Base(title, rect)
		{
			ensureCallIsOnMainThread();

			AppContextBase::notifyWindowMapping(handle(), *this);
			GraphicsSystem::notifyWindowConstruction(*this, handle());
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
			AppContextBase::notifyWindowDestruction(handle());
			GraphicsSystem::notifyWindowDestruction(*this);
		}

		void notifyMove(Window& oldWindow)
		{
			AppContextBase::notifyWindowMapping(handle(), *this);
			GraphicsSystem::notifyWindowReplacement(oldWindow, *this);
		}
	};
}
