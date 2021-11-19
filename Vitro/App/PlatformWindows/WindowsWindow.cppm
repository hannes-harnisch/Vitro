module;
#include "VitroCore/PlatformWindows/WindowsAPI.hpp"

#include <memory>
#include <string_view>
export module vt.App.Windows.Window;

import vt.Core.Rect;
import vt.Core.Vector;
import vt.Core.Windows.Utils;

namespace vt::windows
{
	export class WindowsWindow
	{
	public:
		static constexpr TCHAR const WINDOW_CLASS_NAME[] = TEXT(VT_ENGINE_NAME);

		static constexpr Rectangle DEFAULT_RECT {
			.x		= CW_USEDEFAULT,
			.y		= CW_USEDEFAULT,
			.width	= static_cast<unsigned>(CW_USEDEFAULT),
			.height = static_cast<unsigned>(CW_USEDEFAULT),
		};

	protected:
		WindowsWindow(std::string_view title, Rectangle rect);

		void		open();
		void		close();
		void		maximize();
		void		minimize();
		void		enable_cursor();
		void		disable_cursor();
		void		enable_resize();
		void		disable_resize();
		Extent		get_size() const;
		void		set_size(Extent size);
		Int2		get_position() const;
		void		set_position(Int2 position);
		std::string get_title() const;
		void		set_title(std::string_view title);
		Rectangle	client_area() const;
		HWND		native_handle();

	private:
		struct WindowDeleter
		{
			using pointer = HWND;
			void operator()(HWND hwnd) const
			{
				auto succeeded = ::DestroyWindow(hwnd);
				check_winapi_error(succeeded, "Failed to destroy window.");
			}
		};
		std::unique_ptr<HWND, WindowDeleter> window;

		void make_resizable(bool enable) const;
	};
}
