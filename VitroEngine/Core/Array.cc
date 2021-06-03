module;
#include "Trace/Assert.hh"

#include <algorithm>
#include <iterator>
#include <limits>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <utility>
export module Vitro.Core.Array;

export template<typename T, typename Allocator = std::allocator<T>> class Array
{
	using AllocTraits = std::allocator_traits<Allocator>;

public:
	using ValueType		 = typename AllocTraits::value_type;
	using Reference		 = ValueType&;
	using ConstReference = ValueType const&;
	using Pointer		 = typename AllocTraits::pointer;
	using ConstPointer	 = typename AllocTraits::const_pointer;
	using SizeType		 = typename AllocTraits::size_type;
	using DifferenceType = typename AllocTraits::difference_type;

	constexpr Array() noexcept = default;

	constexpr Array(ValueType arr[], SizeType const count) noexcept : arr(arr), count(count)
	{}

	constexpr Array(SizeType const count) : Array(allocate(count), count)
	{
		if constexpr(std::is_default_constructible_v<ValueType>)
		{
			Allocator alloc;
			for(auto& element : *this)
				AllocTraits::construct(alloc, &element);
		}
	}

	template<typename... Ts> constexpr Array(SizeType const count, Ts&&... ts) : Array(allocate(count), count)
	{
		Allocator alloc;
		for(auto& element : *this)
			AllocTraits::construct(alloc, &element, std::forward<Ts>(ts)...);
	}

	template<typename U>
	constexpr Array(SizeType const count, std::initializer_list<U> initializers) : Array(allocate(count), count)
	{
		veAssert(count >= initializers.size(), "Size of initializer list exceeds array size.");

		Allocator alloc;
		auto element = begin();
		for(auto&& init : initializers)
			AllocTraits::construct(alloc, &*element++, init);

		if constexpr(std::is_default_constructible_v<ValueType>)
			for(auto&& rest : *this | std::views::drop(initializers.size()))
				AllocTraits::construct(alloc, &rest);
	}

	template<typename U, typename V>
	constexpr Array(SizeType count, std::initializer_list<U> initializers, V&& fallback) : Array(count, initializers)
	{
		Allocator alloc;
		for(auto&& element : *this | std::views::drop(initializers.size()))
			AllocTraits::construct(alloc, &element, std::forward<V>(fallback));
	}

	constexpr Array(Array const& that) : Array(allocate(that.count), that.count)
	{
		Allocator alloc;
		auto other = that.begin();
		for(auto& element : *this)
			AllocTraits::construct(alloc, &element, *other++);
	}

	constexpr Array(Array&& that) noexcept : Array()
	{
		swap(that);
	}

	constexpr ~Array()
	{
		destruct();
	}

	constexpr Array& operator=(Array that) noexcept
	{
		swap(that);
		return *this;
	}

	[[nodiscard]] constexpr Reference operator[](SizeType const index) noexcept
	{
		veAssert(index < count, "Index into array was out of range.");
		return arr[index];
	}

	[[nodiscard]] constexpr ConstReference operator[](SizeType const index) const noexcept
	{
		veAssert(index < count, "Index into array was out of range.");
		return arr[index];
	}

	[[nodiscard]] constexpr bool operator==(auto const& that) const noexcept
	{
		return size() == std::size(that) && std::equal(begin(), end(), std::begin(that));
	}

	[[nodiscard]] constexpr bool operator!=(auto const& that) const noexcept
	{
		return !operator==(that);
	}

	[[nodiscard]] constexpr bool operator<(auto const& that) const noexcept
	{
		return std::lexicographical_compare(begin(), end(), std::begin(that), std::end(that));
	}

	[[nodiscard]] constexpr bool operator>(auto const& that) const noexcept
	{
		return that < *this;
	}

	[[nodiscard]] constexpr bool operator<=(auto const& that) const noexcept
	{
		return !operator>(that);
	}

	[[nodiscard]] constexpr bool operator>=(auto const& that) const noexcept
	{
		return !operator<(that);
	}

	[[nodiscard]] constexpr auto operator<=>(auto const& that) const noexcept
	{
		return std::lexicographical_compare_three_way(begin(), end(), std::begin(that), std::end(that));
	}

	[[nodiscard]] constexpr Reference at(SizeType const index)
	{
		if(index < count)
			return arr[index];
		else
			throw std::out_of_range("Index into array was out of range.");
	}

	[[nodiscard]] constexpr ConstReference at(SizeType const index) const
	{
		if(index < count)
			return arr[index];
		else
			throw std::out_of_range("Index into array was out of range.");
	}

	[[nodiscard]] constexpr Pointer get(SizeType const index) noexcept
	{
		if(index < count)
			return arr + index;
		else
			return nullptr;
	}

	[[nodiscard]] constexpr ConstPointer get(SizeType const index) const noexcept
	{
		if(index < count)
			return arr + index;
		else
			return nullptr;
	}

	[[nodiscard]] constexpr Reference front() noexcept
	{
		return arr[0];
	}

	[[nodiscard]] constexpr ConstReference front() const noexcept
	{
		return arr[0];
	}

	[[nodiscard]] constexpr Reference back() noexcept
	{
		return arr[count - 1];
	}

	[[nodiscard]] constexpr ConstReference back() const noexcept
	{
		return arr[count - 1];
	}

	[[nodiscard]] constexpr bool empty() const noexcept
	{
		return !count;
	}

	[[nodiscard]] constexpr SizeType size() const noexcept
	{
		return count;
	}

	[[nodiscard]] constexpr Pointer data() noexcept
	{
		return arr;
	}

	[[nodiscard]] constexpr ConstPointer data() const noexcept
	{
		return arr;
	}

	template<typename U> constexpr void fill(U&& value) noexcept
	{
		std::fill(begin(), end(), std::forward<U>(value));
	}

	[[nodiscard]] constexpr Pointer release() noexcept
	{
		return std::exchange(arr, Pointer());
	}

	constexpr void reset(Pointer const resetValue = Pointer()) noexcept
	{
		destruct();
		arr = resetValue;
	}

	constexpr void swap(Array& that) noexcept
	{
		std::swap(count, that.count);
		std::swap(arr, that.arr);
	}

	friend constexpr void swap(Array& left, Array& right) noexcept
	{
		left.swap(right);
	}

private:
	template<typename V> class ArrayIterator
	{
		friend Array;

	public:
		using iterator_concept = std::contiguous_iterator_tag;
		using value_type	   = V;
		using pointer		   = V*;
		using reference		   = V&;
		using difference_type  = typename AllocTraits::difference_type;

		constexpr ArrayIterator() noexcept = default;

		[[nodiscard]] constexpr V& operator*() const noexcept
		{
			return *pos;
		}

		[[nodiscard]] constexpr V* operator->() const noexcept
		{
			return pos;
		}

		template<typename U> [[nodiscard]] constexpr bool operator==(ArrayIterator<U> const that) const noexcept
		{
			return pos == that.pos;
		}

		template<typename U> [[nodiscard]] constexpr bool operator!=(ArrayIterator<U> const that) const noexcept
		{
			return pos != that.pos;
		}

		template<typename U> [[nodiscard]] constexpr bool operator>(ArrayIterator<U> const that) const noexcept
		{
			return pos > that.pos;
		}

		template<typename U> [[nodiscard]] constexpr bool operator>=(ArrayIterator<U> const that) const noexcept
		{
			return pos >= that.pos;
		}

		template<typename U> [[nodiscard]] constexpr bool operator<(ArrayIterator<U> const that) const noexcept
		{
			return pos < that.pos;
		}

		template<typename U> [[nodiscard]] constexpr bool operator<=(ArrayIterator<U> const that) const noexcept
		{
			return pos <= that.pos;
		}

		template<typename U> [[nodiscard]] constexpr auto operator<=>(ArrayIterator<U> const that) const noexcept
		{
			return pos <=> that.pos;
		}

		constexpr ArrayIterator& operator++() noexcept
		{
			incrementPosition(1);
			return *this;
		}

		constexpr ArrayIterator operator++(int) noexcept
		{
			auto old = *this;
			incrementPosition(1);
			return old;
		}

		constexpr ArrayIterator& operator--() noexcept
		{
			decrementPosition(1);
			return *this;
		}

		constexpr ArrayIterator operator--(int) noexcept
		{
			auto old = *this;
			decrementPosition(1);
			return old;
		}

		constexpr ArrayIterator& operator+=(DifferenceType const offset) noexcept
		{
			incrementPosition(offset);
			return *this;
		}

		[[nodiscard]] constexpr ArrayIterator operator+(DifferenceType const offset) const noexcept
		{
			auto old = *this;
			return old += offset;
		}

		[[nodiscard]] friend constexpr ArrayIterator operator+(DifferenceType const offset,
															   ArrayIterator const iterator) noexcept
		{
			return iterator + offset;
		}

		constexpr ArrayIterator& operator-=(DifferenceType const offset) noexcept
		{
			decrementPosition(offset);
			return *this;
		}

		[[nodiscard]] constexpr ArrayIterator operator-(DifferenceType const offset) const noexcept
		{
			auto old = *this;
			return old -= offset;
		}

		template<typename U> [[nodiscard]] constexpr DifferenceType operator-(ArrayIterator<U> const that) const noexcept
		{
			return pos - that.pos;
		}

		[[nodiscard]] constexpr V& operator[](SizeType const index) const noexcept
		{
			return *(*this + index);
		}

	private:
		V* pos {};
#if VE_DEBUG
		V* begin {};
		V* end {};

		constexpr ArrayIterator(V* const pos, V* const begin, V* const end) noexcept : pos(pos), begin(begin), end(end)
		{}
#else
		constexpr ArrayIterator(V* const pos) noexcept : pos(pos)
		{}
#endif

		constexpr void incrementPosition(DifferenceType const offset) noexcept
		{
			veAssert(pos < end, "Cannot increment iterator past end.");
			pos += offset;
		}

		constexpr void decrementPosition(DifferenceType const offset) noexcept
		{
			veAssert(begin < pos, "Cannot decrement iterator before begin.");
			pos -= offset;
		}
	};

public:
	using Iterator			   = ArrayIterator<ValueType>;
	using ConstIterator		   = ArrayIterator<const ValueType>;
	using ReverseIterator	   = std::reverse_iterator<Iterator>;
	using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

	[[nodiscard]] constexpr Iterator begin() noexcept
	{
#if VE_DEBUG
		return {arr, arr, arr + count};
#else
		return {arr};
#endif
	}

	[[nodiscard]] constexpr ConstIterator begin() const noexcept
	{
#if VE_DEBUG
		return {arr, arr, arr + count};
#else
		return {arr};
#endif
	}

	[[nodiscard]] constexpr Iterator end() noexcept
	{
#if VE_DEBUG
		Pointer const endPos = arr + count;
		return {endPos, arr, endPos};
#else
		return {arr + count};
#endif
	}

	[[nodiscard]] constexpr ConstIterator end() const noexcept
	{
#if VE_DEBUG
		Pointer const endPos = arr + count;
		return {endPos, arr, endPos};
#else
		return {arr + count};
#endif
	}

	[[nodiscard]] constexpr ReverseIterator rbegin() noexcept
	{
		return ReverseIterator(end());
	}

	[[nodiscard]] constexpr ConstReverseIterator rbegin() const noexcept
	{
		return ConstReverseIterator(end());
	}

	[[nodiscard]] constexpr ReverseIterator rend() noexcept
	{
		return ReverseIterator(begin());
	}

	[[nodiscard]] constexpr ConstReverseIterator rend() const noexcept
	{
		return ConstReverseIterator(begin());
	}

	[[nodiscard]] constexpr ConstIterator cbegin() const noexcept
	{
		return begin();
	}

	[[nodiscard]] constexpr ConstIterator cend() const noexcept
	{
		return end();
	}

	[[nodiscard]] constexpr ConstReverseIterator crbegin() const noexcept
	{
		return rbegin();
	}

	[[nodiscard]] constexpr ConstReverseIterator crend() const noexcept
	{
		return rend();
	}

private:
	Pointer arr {};
	SizeType count {};

	static constexpr Pointer allocate(SizeType const count)
	{
		Allocator alloc;
		return AllocTraits::allocate(alloc, count);
	}

	constexpr void destruct() noexcept
	{
		Allocator alloc;

		if constexpr(!std::is_trivially_destructible_v<ValueType>)
			for(auto& element : *this)
				AllocTraits::destroy(alloc, &element);

		AllocTraits::deallocate(alloc, arr, count);
	}
};
