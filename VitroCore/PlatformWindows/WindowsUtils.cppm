module;
#include "VitroCore/Macros.hpp"
#include "WindowsAPI.hpp"

#include <format>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <system_error>
export module vt.Core.Windows.Utils;

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

	export std::wstring widen_string(std::string_view in);
	export std::string	narrow_string(std::wstring_view in);
	export void			show_error_message_box(std::string_view title, std::string_view message);
}
