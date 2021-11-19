module;
#include "VitroCore/PlatformWindows/WindowsAPI.hpp"
export module vt.App.Windows.AppContext;

import vt.App.AppContextBase;
import vt.App.WindowEvent;
import vt.Core.Vector;

namespace vt::windows
{
	export class WindowsAppContext : public AppContextBase
	{
	protected:
		WindowsAppContext();

		void pump_system_events();

	private:
		unsigned key_repeats		 = 0;
		KeyCode	 last_key_code		 = KeyCode::None;
		Int2	 last_mouse_position = {};

		static WindowsAppContext& get()
		{
			return static_cast<WindowsAppContext&>(AppContextBase::get());
		}

		static LRESULT CALLBACK forward_messages(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param);

		void					  on_window_move(HWND hwnd, LPARAM l_param) const;
		void					  on_window_size(HWND hwnd, LPARAM l_param) const;
		void					  restore_window_cursor_state(HWND hwnd, WPARAM w_param) const;
		template<typename E> void on_window_event(HWND hwnd) const;
		void					  on_window_close(HWND hwnd) const;
		void					  on_window_show(HWND hwnd, WPARAM w_param) const;
		void					  on_raw_input(HWND hwnd, LPARAM l_param) const;
		void					  on_key_down(HWND hwnd, WPARAM w_param);
		void					  on_key_up(HWND hwnd, WPARAM w_param);
		void					  on_key_text(HWND hwnd, WPARAM w_param) const;
		void					  store_last_mouse_position(LPARAM l_param);
		template<typename E> void on_mouse_event(HWND hwnd, MouseCode button) const;
		void					  on_vertical_scroll(HWND hwnd, WPARAM w_param) const;
		void					  on_horizontal_scroll(HWND hwnd, WPARAM w_param) const;
	};
}
