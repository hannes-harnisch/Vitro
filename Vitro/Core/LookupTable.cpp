module;
#include <algorithm>
#include <array>
#include <type_traits>
export module vt.Core.LookupTable;

import vt.Core.Enum;

namespace vt
{
	// Represents a simple array-based lookup table. Return it from an immediately invoked lambda expression to initialize a
	// constexpr lookup table. The size will be deduced from the key type if it is an enum, otherwise it must be manually
	// provided. Looking up a value that is in the numeric range of zero to the largest key, but wasn't explicitly assigned,
	// will return zero.
	export template<typename Key, typename Value, size_t COUNT = size_from_enum_max<Key>()> class LookupTable
	{
	public:
		constexpr Value& operator[](Key key)
		{
			return table[static_cast<size_t>(key)];
		}

		constexpr Value operator[](Key key) const
		{
			return table[static_cast<size_t>(key)];
		}

	private:
		Value table[COUNT] = {};
	};
}
