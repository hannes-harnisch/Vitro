module;
#include "Windows.API.hh"

#include <string_view>
export module Vitro.Windows.StringUtils;

namespace Windows
{
	export std::wstring widenString(std::string_view in)
	{
		const int inLength	= static_cast<int>(in.length());
		const int outLength = ::MultiByteToWideChar(CP_UTF8, 0, in.data(), inLength, nullptr, 0);
		std::wstring out(outLength, L'\0');
		::MultiByteToWideChar(CP_UTF8, 0, in.data(), inLength, out.data(), outLength);
		return out;
	}

	export std::string narrowString(std::wstring_view in)
	{
		const int inLength	= static_cast<int>(in.length());
		const int outLength = ::WideCharToMultiByte(CP_UTF8, 0, in.data(), inLength, nullptr, 0, nullptr, nullptr);
		std::string out(outLength, '\0');
		::WideCharToMultiByte(CP_UTF8, 0, in.data(), inLength, out.data(), outLength, nullptr, nullptr);
		return out;
	}
}
