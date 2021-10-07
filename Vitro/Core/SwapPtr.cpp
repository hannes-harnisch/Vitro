module;
#include <utility>
export module vt.Core.SwapPtr;

namespace vt
{
	// Utility for making RAII easier when using unique_ptr custom deleters is too cumbersome. It implements the exchanging
	// behavior of unique_ptr, but the destructor still needs to be defined manually in the owner type.
	export template<typename T> class SwapPtr
	{
	public:
		SwapPtr() = default;

		explicit SwapPtr(T ptr) noexcept : ptr(ptr)
		{}

		SwapPtr(SwapPtr&& that) noexcept : ptr(std::exchange(that.ptr, T()))
		{}

		SwapPtr& operator=(SwapPtr&& that) noexcept
		{
			std::swap(ptr, that.ptr);
			return *this;
		}

		T get() const noexcept
		{
			return ptr;
		}

		void set(T that) noexcept
		{
			ptr = that;
		}

		SwapPtr(SwapPtr const&) = delete;
		SwapPtr& operator=(SwapPtr const&) = delete;

	private:
		T ptr = {};
	};
}
