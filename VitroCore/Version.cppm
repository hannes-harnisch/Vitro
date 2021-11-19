module;
#include <compare>
#include <string_view>
export module vt.Core.Version;

namespace vt
{
	export struct Version
	{
		unsigned major = 0;
		unsigned minor = 0;
		unsigned patch = 0;

		auto operator<=>(Version const&) const = default;

		// Parse a version object from a string of the form "7_25_134" and similar where _ is an arbitrary delimiting character.
		static Version from_delimited_string(std::string_view version_string, char delimiter);

		std::string to_string() const;
	};
}
