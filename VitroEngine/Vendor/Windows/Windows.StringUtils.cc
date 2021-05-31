module;
#include "Windows.API.hh"

#include <string_view>
export module Vitro.Windows.StringUtils;

namespace Windows
{
	export std::wstring widenString(std::string_view const in)
	{
		int const inLength	= static_cast<int>(in.length());
		int const outLength = ::MultiByteToWideChar(CP_UTF8, 0, in.data(), inLength, nullptr, 0);
		std::wstring out(outLength, L'\0');
		::MultiByteToWideChar(CP_UTF8, 0, in.data(), inLength, out.data(), outLength);
		return out;
	}

	export std::string narrowString(std::wstring_view const in)
	{
		int const inLength	= static_cast<int>(in.length());
		int const outLength = ::WideCharToMultiByte(CP_UTF8, 0, in.data(), inLength, nullptr, 0, nullptr, nullptr);
		std::string out(outLength, '\0');
		::WideCharToMultiByte(CP_UTF8, 0, in.data(), inLength, out.data(), outLength, nullptr, nullptr);
		return out;
	}
}
