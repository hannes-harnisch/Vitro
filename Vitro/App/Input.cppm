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
		static bool is_down(KeyCode key);
		static bool is_down(MouseCode button);
		static Int2 mouse_position();

	private:
		AtomicEnumBitArray<KeyCode>	  key_down_states;
		AtomicEnumBitArray<MouseCode> mouse_down_states;
		std::atomic<Int2>			  mouse_pos;

		Input();

		void on_key_down(KeyDownEvent& event);
		void on_key_up(KeyUpEvent& event);
		void on_mouse_down(MouseDownEvent& event);
		void on_mouse_up(MouseUpEvent& event);
		void on_mouse_move(MouseMoveEvent& event);
	};
}
