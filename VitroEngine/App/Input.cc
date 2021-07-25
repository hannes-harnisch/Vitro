module;
#include "Core/Enum.hh"

#include <bitset>
export module Vitro.App.Input;

import Vitro.App.EventBinding;
import Vitro.App.KeyCode;
import Vitro.App.MouseCode;
import Vitro.App.WindowEvent;
import Vitro.Core.Singleton;
import Vitro.Core.Vector;

namespace vt
{
	export class Input : public Singleton<Input>
	{
		friend class AppSystem;

	public:
		static bool isDown(KeyCode key)
		{
			return get().keyDownStates[static_cast<size_t>(key)];
		}

		static bool isDown(MouseCode button)
		{
			return get().mouseDownStates[static_cast<size_t>(button)];
		}

		static Int2 mousePosition()
		{
			return get().mousePos;
		}

	private:
		std::bitset<sizeFromEnumMax<KeyCode>()> keyDownStates;
		std::bitset<sizeFromEnumMax<MouseCode>()> mouseDownStates;
		Int2 mousePos;
		EventBinding eventBinding;

		Input() :
			eventBinding(this, &Input::onKeyDown, &Input::onKeyUp, &Input::onMouseDown, &Input::onMouseUp, &Input::onMouseMove)
		{}

		void onKeyDown(KeyDownEvent& e)
		{
			keyDownStates[static_cast<size_t>(e.key)] = true;
		}

		void onKeyUp(KeyUpEvent& e)
		{
			keyDownStates[static_cast<size_t>(e.key)] = false;
		}

		void onMouseDown(MouseDownEvent& e)
		{
			mouseDownStates[static_cast<size_t>(e.button)] = true;
		}

		void onMouseUp(MouseUpEvent& e)
		{
			mouseDownStates[static_cast<size_t>(e.button)] = false;
		}

		void onMouseMove(MouseMoveEvent& e)
		{
			mousePos = e.position;
		}
	};
}
