module;
#include <string>
#include <string_view>
export module Vitro.Graphics.AdapterBase;

namespace vt
{
	export class AdapterBase
	{
	public:
		std::string_view getName() const
		{
			return name;
		}

		size_t getVRAM() const
		{
			return vram;
		}

	protected:
		std::string name;
		size_t vram;

		AdapterBase() = default;
	};
}
