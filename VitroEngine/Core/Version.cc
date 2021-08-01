module;
#include <format>
#include <string>
export module Vitro.Core.Version;

namespace vt
{
	export struct Version
	{
		unsigned major = 0;
		unsigned minor = 0;
		unsigned patch = 0;

		std::string toString() const
		{
			return std::format("{}.{}.{}", major, minor, patch);
		}
	};
}
