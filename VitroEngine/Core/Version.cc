module;
#include <format>
#include <string>
export module vt.Core.Version;

namespace vt
{
	export struct Version
	{
		unsigned major = 0;
		unsigned minor = 0;
		unsigned patch = 0;

		std::string to_string() const
		{
			return std::format("{}.{}.{}", major, minor, patch);
		}
	};
}
