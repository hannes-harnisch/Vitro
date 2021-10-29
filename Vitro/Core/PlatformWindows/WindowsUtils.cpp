module;
#include "Core/Macros.hpp"
#include "WindowsAPI.hpp"

#include <format>
#include <stdexcept>
#include <string_view>
#include <system_error>
export module vt.Core.Windows.Utils;

import vt.Core.Scope;

namespace vt::windows
{
	// Utility for throwing exceptions if a Win32 API call fails. Before using it, check the documentation on whether a given
	// Win32 function actually reports errors retrievable through GetLastError.
	export template<auto WIN32_FUNC> auto call_win32(char const* error_msg, auto... args)
	{
		::SetLastError(0);
		ScopeExit throw_on_error = [error_msg] {
			auto error = ::GetLastError();
			if(error != ERROR_SUCCESS)
			{
				auto msg = std::format("{} Reason: {}", error_msg, std::system_category().message(error));
				throw std::runtime_error(msg);
			}
		};
		return WIN32_FUNC(args...);
	}

	export std::wstring widen_string(std::string_view in)
	{
		int in_length  = static_cast<int>(in.length());
		int out_length = call_win32<::MultiByteToWideChar>("Failed to query for string length.", CP_UTF8, 0, in.data(),
														   in_length, nullptr, 0);

		std::wstring out(out_length, L'\0');
		call_win32<::MultiByteToWideChar>("Failed to write converted string.", CP_UTF8, 0, in.data(), in_length, out.data(),
										  out_length);
		return out;
	}

	export std::string narrow_string(std::wstring_view in)
	{
		int in_length  = static_cast<int>(in.length());
		int out_length = call_win32<::WideCharToMultiByte>("Failed to query for string length.", CP_UTF8, 0, in.data(),
														   in_length, nullptr, 0, nullptr, nullptr);

		std::string out(out_length, '\0');
		call_win32<::WideCharToMultiByte>("Failed to write converted string.", CP_UTF8, 0, in.data(), in_length, out.data(),
										  out_length, nullptr, nullptr);
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
