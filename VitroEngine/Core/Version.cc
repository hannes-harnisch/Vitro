module;
#include <cstdint>
#include <format>
export module Vitro.Core.Version;

namespace vt
{
	export struct Version
	{
		uint16_t major = 0;
		uint16_t minor = 0;
		uint16_t patch = 0;

		std::string toString() const
		{
			return std::format("{}.{}.{}", major, minor, patch);
		}
	};
}
