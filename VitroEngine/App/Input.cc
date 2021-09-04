﻿module;
#include <new>
export module vt.App.Input;

import vt.App.EventListener;
import vt.App.KeyCode;
import vt.App.MouseCode;
import vt.App.WindowEvent;
import vt.Core.Enum;
import vt.Core.Singleton;
import vt.Core.Vector;

namespace vt
{
	export class Input : public Singleton<Input>, public EventListener
	{
		friend class AppSystem;

	public:
		static bool is_down(KeyCode key)
		{
			return get().key_down_states[key];
		}

		static bool is_down(MouseCode button)
		{
			return get().mouse_down_states[button];
		}

		static Int2 mouse_position()
		{
			return get().mouse_pos;
		}

	private:
		EnumBitArray<KeyCode>	key_down_states;
		EnumBitArray<MouseCode> mouse_down_states;
		Int2					mouse_pos;

		Input()
		{
			register_event_handlers<&Input::on_key_down, &Input::on_key_up, &Input::on_mouse_down, &Input::on_mouse_up,
									&Input::on_mouse_move>();
		}

		void on_key_down(KeyDownEvent& event)
		{
			key_down_states[event.key] = true;
		}

		void on_key_up(KeyUpEvent& event)
		{
			key_down_states[event.key] = false;
		}

		void on_mouse_down(MouseDownEvent& event)
		{
			mouse_down_states[event.button] = true;
		}

		void on_mouse_up(MouseUpEvent& event)
		{
			mouse_down_states[event.button] = false;
		}

		void on_mouse_move(MouseMoveEvent& event)
		{
			mouse_pos = event.position;
		}
	};
}
