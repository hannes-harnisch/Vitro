#pragma once

#include "Vitro/Client/ClientBase.h"
#include "Vitro/Client/KeyCode.h"
#include "Vitro/Client/MouseCode.h"
#include "Vitro/Math/Vector.h"

namespace Vitro
{
	class Input final
	{
		friend ClientBase;

	public:
		static bool IsDown(KeyCode key)
		{
			return KeyStates[key];
		}

		static bool IsDown(MouseCode button)
		{
			return MouseStates[button];
		}

		static Int2 GetMousePosition()
		{
			return MousePosition;
		}

		static int GetMouseX()
		{
			return MousePosition.X;
		}

		static int GetMouseY()
		{
			return MousePosition.Y;
		}

		Input() = delete;

	private:
		static inline std::unordered_map<KeyCode, bool> KeyStates;
		static inline std::unordered_map<MouseCode, bool> MouseStates;
		static inline Int2 MousePosition;
	};
}
