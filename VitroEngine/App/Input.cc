module;
#include <new>
export module Vitro.App.Input;

import Vitro.App.EventListener;
import Vitro.App.KeyCode;
import Vitro.App.MouseCode;
import Vitro.App.WindowEvent;
import Vitro.Core.Enum;
import Vitro.Core.Singleton;
import Vitro.Core.Vector;

namespace vt
{
	export class Input : public Singleton<Input>, public EventListener
	{
		friend class AppSystem;

	public:
		static bool isDown(KeyCode key)
		{
			return get().keyDownStates[key];
		}

		static bool isDown(MouseCode button)
		{
			return get().mouseDownStates[button];
		}

		static Int2 mousePosition()
		{
			return get().mousePos;
		}

	private:
		EnumBitArray<KeyCode>	keyDownStates;
		EnumBitArray<MouseCode> mouseDownStates;
		Int2					mousePos;

		Input()
		{
			registerEventHandlers<&Input::onKeyDown, &Input::onKeyUp, &Input::onMouseDown, &Input::onMouseUp,
								  &Input::onMouseMove>();
		}

		void onKeyDown(KeyDownEvent& e)
		{
			keyDownStates[e.key] = true;
		}

		void onKeyUp(KeyUpEvent& e)
		{
			keyDownStates[e.key] = false;
		}

		void onMouseDown(MouseDownEvent& e)
		{
			mouseDownStates[e.button] = true;
		}

		void onMouseUp(MouseUpEvent& e)
		{
			mouseDownStates[e.button] = false;
		}

		void onMouseMove(MouseMoveEvent& e)
		{
			mousePos = e.position;
		}
	};
}
