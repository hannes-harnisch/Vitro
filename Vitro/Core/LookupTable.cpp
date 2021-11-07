module;
#include <algorithm>
#include <array>
#include <type_traits>
export module vt.Core.LookupTable;

namespace vt
{
	template<typename Key> consteval size_t count_lut_mappings(std::initializer_list<Key> mappings)
	{
		auto max = std::max_element(mappings.begin(), mappings.end(), [](Key left, Key right) {
			return static_cast<size_t>(left) < static_cast<size_t>(right);
		});
		return static_cast<size_t>(*max) + 1;
	}

	template<typename Key, typename Value, size_t COUNT>
	consteval void set_lut_element(Key key, Value value, std::array<Value, COUNT>& lut, std::array<bool, COUNT>& assigned)
	{
		using KeyIntegerType = std::conditional_t<std::is_enum_v<Key>, std::underlying_type_t<Key>, Key>;
		if constexpr(std::is_signed_v<KeyIntegerType>)
			if(static_cast<KeyIntegerType>(key) < 0)
				throw "Negative keys are not allowed.";

		size_t index = static_cast<size_t>(key);

		if(assigned[index])
			throw "Duplicate keys are not allowed in lookup table.";

		lut[index]		= value;
		assigned[index] = true;
	}

	template<typename T1, typename T2> struct Pair
	{
		T1 key;
		T2 value;
	};

	constexpr inline size_t MAX_LOOKUP_TABLE_SIZE = 16384;

	// Generates a lookup table from Key to Value in the form of a std::array. The size of the table will be such that all keys
	// must be unique and non-negative, otherwise the attempt to generate the table will fail by throwing an exception. Looking
	// up a value that is in the numeric range of zero to the largest key, but wasn't explicitly provided in the template
	// arguments, will return zero.
	export template<typename Key, typename Value, Pair<Key, Value>... MAP> consteval auto make_lookup_table()
	{
		constexpr size_t COUNT = count_lut_mappings({MAP.key...});

		if constexpr(sizeof(Value[COUNT]) > MAX_LOOKUP_TABLE_SIZE)
			static_assert(false, "The lookup table would take up more than 16 kB of space. Consider using something else.");

		if constexpr(COUNT / sizeof...(MAP) >= 4)
			static_assert(false, "At least 75% of the lookup table space is wasted. Consider using something else.");

		std::array<Value, COUNT> lut {};
		std::array<bool, COUNT>	 assigned {};

		(set_lut_element(MAP.key, MAP.value, lut, assigned), ...);
		return lut;
	}
}
