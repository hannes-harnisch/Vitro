module;
#include <cstdint>
#include <format>
export module Vitro.Core.Version;

export struct Version
{
	uint16_t major {};
	uint16_t minor {};
	uint16_t patch {};

	std::string toString() const
	{
		return std::format("{}.{}.{}", major, minor, patch);
	}
};
