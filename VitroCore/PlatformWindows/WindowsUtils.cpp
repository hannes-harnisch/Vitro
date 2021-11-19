module;
#include "VitroCore/Macros.hpp"
#include "WindowsAPI.hpp"

#include <string_view>
module vt.Core.Windows.Utils;

namespace vt::windows
{
	std::wstring widen_string(std::string_view in)
	{
		int in_length  = static_cast<int>(in.length()) + 1;
		int out_length = ::MultiByteToWideChar(CP_UTF8, 0, in.data(), in_length, nullptr, 0);
		check_winapi_error(out_length, "Failed to query for string length.");

		std::wstring out(out_length, L'\0');
		out_length = ::MultiByteToWideChar(CP_UTF8, 0, in.data(), in_length, out.data(), out_length);
		check_winapi_error(out_length, "Failed to write widened string.");
		return out;
	}

	std::string narrow_string(std::wstring_view in)
	{
		int in_length  = static_cast<int>(in.length()) + 1;
		int out_length = ::WideCharToMultiByte(CP_UTF8, 0, in.data(), in_length, nullptr, 0, nullptr, nullptr);
		check_winapi_error(out_length, "Failed to query for string length.");

		std::string out(out_length, '\0');
		out_length = ::WideCharToMultiByte(CP_UTF8, 0, in.data(), in_length, out.data(), out_length, nullptr, nullptr);
		check_winapi_error(out_length, "Failed to write narrowed string.");
		return out;
	}

	void show_error_message_box(std::string_view title, std::string_view message)
	{
		auto wide_title = widen_string(title);
		auto wide_msg	= widen_string(message);

		int result = ::MessageBox(nullptr, wide_msg.data(), wide_title.data(), MB_OK | MB_ICONERROR);
		VT_ASSERT(result, "Failed to show message box.");
	}
}
