module;
#include "VitroCore/Macros.hpp"
#include "WindowsAPI.hpp"

#include <format>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <system_error>
export module vt.Core.Windows.Utils;

import vt.Core.Scope;

namespace vt::windows
{
	template<typename T> struct ComDeleter
	{
		void operator()(T* com_interface) const
		{
			com_interface->Release();
		}
	};
	export template<typename T> using ComUnique = std::unique_ptr<T, ComDeleter<T>>;

	// Utility for throwing an exception with a detailed message if a Windows API call fails. Before using it, check Windows API
	// documentation on whether GetLastError even reports an error for the function you want to call and especially if the
	// SetLastError API function should be called before it to distinguish between valid zero returns and an actual error.
	export template<typename T> void check_winapi_error(T result_value, char const* error_msg, T error_value = {})
	{
		if(result_value != error_value)
			return;

		auto error = ::GetLastError();
		if(error != ERROR_SUCCESS)
		{
			auto msg = std::format("{} Reason: {}", error_msg, std::system_category().message(error));
			VT_DEBUG_BREAK();
			throw std::runtime_error(msg);
		}
	}

	export std::wstring widen_string(std::string_view in)
	{
		int in_length  = static_cast<int>(in.length()) + 1;
		int out_length = ::MultiByteToWideChar(CP_UTF8, 0, in.data(), in_length, nullptr, 0);
		check_winapi_error(out_length, "Failed to query for string length.");

		std::wstring out(out_length, L'\0');
		out_length = ::MultiByteToWideChar(CP_UTF8, 0, in.data(), in_length, out.data(), out_length);
		check_winapi_error(out_length, "Failed to write widened string.");
		return out;
	}

	export std::string narrow_string(std::wstring_view in)
	{
		int in_length  = static_cast<int>(in.length()) + 1;
		int out_length = ::WideCharToMultiByte(CP_UTF8, 0, in.data(), in_length, nullptr, 0, nullptr, nullptr);
		check_winapi_error(out_length, "Failed to query for string length.");

		std::string out(out_length, '\0');
		out_length = ::WideCharToMultiByte(CP_UTF8, 0, in.data(), in_length, out.data(), out_length, nullptr, nullptr);
		check_winapi_error(out_length, "Failed to write narrowed string.");
		return out;
	}

	export void show_error_message_box(std::string_view title, std::string_view message)
	{
		auto wide_title = widen_string(title);
		auto wide_msg	= widen_string(message);

		int result = ::MessageBox(nullptr, wide_msg.data(), wide_title.data(), MB_OK | MB_ICONERROR);
		VT_ASSERT(result, "Failed to show message box.");
	}
}
