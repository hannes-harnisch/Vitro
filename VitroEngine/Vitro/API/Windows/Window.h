#pragma once

#include "Vitro/Client/Window.h"

#include <windows.h>

namespace Vitro::Windows
{
	class Window final : public Vitro::Window
	{
	public:
		Window(int width, int height, int x, int y, const std::string& title);
		Window(Window&& other) noexcept;
		~Window() override;
		Window& operator=(Window&& other) noexcept;

		int GetWidth() const override;
		void SetWidth(int width) override;
		int GetViewportWidth() const override;
		int GetHeight() const override;
		void SetHeight(int height) override;
		int GetViewportHeight() const override;
		int GetX() const override;
		void SetX(int x) override;
		int GetY() const override;
		void SetY(int y) override;
		std::string GetTitle() const override;
		void SetTitle(const std::string& title) override;
		void* GetNativeHandle() const override;
		void Open() override;
		void Close() override;
		void Maximize() override;
		void Minimize() override;
		void TrapCursor(bool shouldTrap) override;

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

	private:
		HWND NativeHandle;
	};
}
