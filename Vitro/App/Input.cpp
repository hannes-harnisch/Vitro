module;
#include <atomic>
export module vt.App.Input;

import vt.App.EventListener;
import vt.App.WindowEvent;
import vt.Core.Enum;
import vt.Core.Singleton;
import vt.Core.Vector;

namespace vt
{
	// Provides thread-safe read access to the global input state.
	export class Input : public Singleton<Input>, public EventListener
	{
		friend class AppSystem;

	public:
		static bool is_down(KeyCode key)
		{
			return get().key_down_states.load()[key];
		}

		static bool is_down(MouseCode button)
		{
			return get().mouse_down_states.load()[button];
		}

		static Int2 mouse_position()
		{
			return get().mouse_pos;
		}

	private:
		std::atomic<EnumBitArray<KeyCode>>	 key_down_states;
		std::atomic<EnumBitArray<MouseCode>> mouse_down_states;
		std::atomic<Int2>					 mouse_pos;

		Input()
		{
			register_event_handlers<&Input::on_key_down, &Input::on_key_up, &Input::on_mouse_down, &Input::on_mouse_up,
									&Input::on_mouse_move>();
		}

		void on_key_down(KeyDownEvent& event)
		{
			set_key_state(event.key, true);
		}

		void on_key_up(KeyUpEvent& event)
		{
			set_key_state(event.key, false);
		}

		void on_mouse_down(MouseDownEvent& event)
		{
			set_mouse_state(event.button, true);
		}

		void on_mouse_up(MouseUpEvent& event)
		{
			set_mouse_state(event.button, false);
		}

		void on_mouse_move(MouseMoveEvent& event)
		{
			mouse_pos = event.position;
		}

		void set_key_state(KeyCode key, bool state)
		{
			// TODO: synchronize
			auto states = key_down_states.load();
			states.set(key, state);
			key_down_states.store(states);
		}

		void set_mouse_state(MouseCode button, bool state)
		{
			// TODO: synchronize
			auto states = mouse_down_states.load();
			states.set(button, state);
			mouse_down_states.store(states);
		}
	};
}
