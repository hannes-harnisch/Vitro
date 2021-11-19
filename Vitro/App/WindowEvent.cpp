module;
#include <format>
module vt.App.WindowEvent;

import vt.Core.Enum;

namespace vt
{
	std::string WindowEvent::to_string() const
	{
		auto handle = reinterpret_cast<uintptr_t>(window.native_handle());
		return std::format("Window({})", handle);
	}

	std::string WindowSizeEvent::to_string() const
	{
		return std::format("{}, Size({})", WindowEvent::to_string(), size.to_string());
	}

	std::string WindowMoveEvent::to_string() const
	{
		return std::format("{}, Position({})", WindowEvent::to_string(), position.to_string());
	}

	std::string KeyEvent::to_string() const
	{
		return std::format("{}, Key({})", WindowEvent::to_string(), enum_name(key));
	}

	std::string KeyDownEvent::to_string() const
	{
		return std::format("{}, Repeats({})", KeyEvent::to_string(), repeats);
	}

	std::string KeyTextEvent::to_string() const
	{
		return std::format("{}, Text({})", KeyEvent::to_string(), text);
	}

	std::string MouseMoveEvent::to_string() const
	{
		return std::format("{}, Position({}), Direction({})", WindowEvent::to_string(), position.to_string(),
						   direction.to_string());
	}

	std::string MouseScrollEvent::to_string() const
	{
		return std::format("{}, Offset({})", WindowEvent::to_string(), offset.to_string());
	}

	std::string MouseClickEvent::to_string() const
	{
		return std::format("{}, Button({})", WindowEvent::to_string(), enum_name(button));
	}
}
