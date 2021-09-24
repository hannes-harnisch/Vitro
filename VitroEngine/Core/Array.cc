module;
#include "Core/Macros.hh"

#if VT_DEBUG
	#define DEBUG 1
#endif
#define HH_ASSERT(condition, message) VT_ASSERT_PURE(condition, message)
#include <Array/Include/Array.hpp>

#include <ranges>
#include <span>
#include <stdexcept>
export module vt.Core.Array;

namespace vt
{
	export template<typename T> using Array = hh::Array<T>;

	// Non-empty non-writable view into a contiguous range of elements. Will also bind to a single element.
	export template<typename T> class ArrayView : public std::span<T const>
	{
		using Base = std::span<T const>;

	public:
		ArrayView(std::contiguous_iterator auto it, size_t count) : Base(it, count)
		{
			if(count == 0)
				on_zero_size();
		}

		template<std::contiguous_iterator It> ArrayView(It first, It last) : Base(first, last)
		{
			if(first == last)
				on_zero_size();
		}

		ArrayView(T const& object) noexcept : Base(&object, 1)
		{}

		ArrayView(std::ranges::contiguous_range auto const& container) : Base(container)
		{
			if(std::empty(container))
				on_zero_size();
		}

		ArrayView(T const&&)								  = delete;
		ArrayView(std::ranges::contiguous_range auto const&&) = delete;

	private:
		static void on_zero_size()
		{
			throw std::invalid_argument("ArrayView cannot be of size zero.");
		}
	};

	// Non-writable view into a contiguous range of elements. Will also bind to a single element.
	export template<typename T> class ConstSpan : public std::span<T const>
	{
		using Base = std::span<T const>;

	public:
		using Base::Base;

		ConstSpan(T const& object) noexcept : Base(&object, 1)
		{}

		ConstSpan(T const&&) = delete;
	};
}
