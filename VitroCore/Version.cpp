module;
#include <charconv>
#include <format>
#include <string_view>
export module vt.Core.Version;

import vt.Core.Algorithm;

namespace vt
{
	export struct Version
	{
		unsigned major = 0;
		unsigned minor = 0;
		unsigned patch = 0;

		auto operator<=>(Version const&) const = default;

		// Parse a version object from a string of the form "7_25_134" and similar where _ is an arbitrary delimiting character.
		static Version from_delimited_string(std::string_view version_string, char delimiter)
		{
			constexpr size_t PARTS = 3;

			auto	  tokens = split<PARTS>(version_string, delimiter);
			unsigned  parts[PARTS];
			unsigned* it = parts;
			for(auto token : tokens)
			{
				auto result = std::from_chars(token.data(), token.data() + token.size(), *it++);
				if(result.ec != std::errc())
				{
					auto msg = std::format("Failed to parse version string with code {}.", std::to_underlying(result.ec));
					throw std::invalid_argument(msg);
				}
			}
			return {
				parts[0],
				parts[1],
				parts[2],
			};
		}

		std::string to_string() const
		{
			return std::format("{}.{}.{}", major, minor, patch);
		}
	};
}
