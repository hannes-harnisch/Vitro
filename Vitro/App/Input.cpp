module;
#include <atomic>
module vt.App.Input;

import vt.App.EventListener;
import vt.App.WindowEvent;
import vt.Core.Enum;
import vt.Core.Singleton;
import vt.Core.Vector;

namespace vt
{
	bool Input::is_down(KeyCode key)
	{
		return get().key_down_states[key];
	}

	bool Input::is_down(MouseCode button)
	{
		return get().mouse_down_states[button];
	}

	Int2 Input::mouse_position()
	{
		return get().mouse_pos;
	}

	Input::Input()
	{
		register_event_handlers<&Input::on_key_down, &Input::on_key_up, &Input::on_mouse_down, &Input::on_mouse_up,
								&Input::on_mouse_move>();
	}

	void Input::on_key_down(KeyDownEvent& event)
	{
		key_down_states.set(event.key, true);
	}

	void Input::on_key_up(KeyUpEvent& event)
	{
		key_down_states.set(event.key, false);
	}

	void Input::on_mouse_down(MouseDownEvent& event)
	{
		mouse_down_states.set(event.button, true);
	}

	void Input::on_mouse_up(MouseUpEvent& event)
	{
		mouse_down_states.set(event.button, false);
	}

	void Input::on_mouse_move(MouseMoveEvent& event)
	{
		mouse_pos = event.position;
	}
}
