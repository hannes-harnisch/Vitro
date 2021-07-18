module;
#include <type_traits>
#include <utility>
export module Vitro.Core.Unique;

namespace vt
{
	template<typename T> void defaultDelete(T* ptr)
	{
		delete ptr;
	}

	template<typename> struct DeleterTraits;
	template<typename TReturn, typename TParam> struct DeleterTraits<TReturn (*)(TParam)>
	{
		using TargetType = TParam;
	};

	export template<typename T, auto Delete = defaultDelete<T>> class [[nodiscard]] Unique
	{
		template<typename, auto> friend class Unique;

		using TDeleterTarget = typename DeleterTraits<decltype(Delete)>::TargetType;
		using TPointer = std::conditional_t<std::is_base_of_v<std::remove_pointer_t<TDeleterTarget>, T>, T*, TDeleterTarget>;

	public:
		template<typename... Ts> static [[nodiscard]] Unique from(Ts&&... ts)
		{
			return new T(std::forward<Ts>(ts)...);
		}

		Unique() = default;

		Unique(std::nullptr_t) noexcept
		{}

		Unique(TPointer ptr) noexcept : ptr(ptr)
		{}

		Unique(Unique&& that) noexcept : ptr(that.release())
		{}

		template<typename TOther, auto DeleteOther> Unique(Unique<TOther, DeleteOther>&& that) noexcept : ptr(that.release())
		{}

		~Unique()
		{
			destroy();
		}

		Unique& operator=(std::nullptr_t) noexcept
		{
			reset();
			return *this;
		}

		Unique& operator=(TPointer const that) noexcept
		{
			reset(that);
			return *this;
		}

		Unique& operator=(Unique&& that) noexcept
		{
			reset(that.release());
			return *this;
		}

		template<typename TOther, auto DeleteOther> Unique& operator=(Unique<TOther, DeleteOther>&& that) noexcept
		{
			reset(that.release());
			return *this;
		}

		[[nodiscard]] std::strong_ordering operator<=>(TPointer const that) const noexcept
		{
			return ptr <=> that;
		}

		[[nodiscard]] TPointer* operator&() noexcept
		{
			return &ptr;
		}

		[[nodiscard]] TPointer const* operator&() const noexcept
		{
			return &ptr;
		}

		[[nodiscard]] decltype(auto) operator*() const noexcept
		{
			return *ptr;
		}

		[[nodiscard]] TPointer operator->() const noexcept
		{
			return ptr;
		}

		[[nodiscard]] decltype(auto) operator->*(auto const memberObjectPtr) const noexcept
			requires(std::is_member_object_pointer_v<decltype(memberObjectPtr)>)
		{
			return ptr->*memberObjectPtr;
		}

		[[nodiscard]] auto operator->*(auto const memberFunctionPtr) const noexcept
			requires(std::is_member_function_pointer_v<decltype(memberFunctionPtr)>)
		{
			return [this, memberFunctionPtr](auto&&... args) {
				return (ptr->*memberFunctionPtr)(std::forward<decltype(args)>(args)...);
			};
		}

		[[nodiscard]] operator TPointer() const noexcept
		{
			return ptr;
		}

		[[nodiscard]] operator bool() const noexcept
		{
			return ptr;
		}

		[[nodiscard]] TPointer get() const noexcept
		{
			return ptr;
		}

		[[nodiscard]] TPointer release() noexcept
		{
			return std::exchange(ptr, nullptr);
		}

		void reset(TPointer const resetValue = nullptr) noexcept
		{
			destroy();
			ptr = resetValue;
		}

		void swap(Unique& that) noexcept
		{
			std::swap(ptr, that.ptr);
		}

		friend void swap(Unique& left, Unique& right) noexcept
		{
			left.swap(right);
		}

	private:
		TPointer ptr = nullptr;

		void destroy() noexcept
		{
			if(ptr)
				Delete(ptr);
		}
	};
}
