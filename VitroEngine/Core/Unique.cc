module;
#include <utility>
export module Vitro.Core.Unique;

export template<typename T> void defaultDelete(T* ptr)
{
	delete ptr;
}

template<typename TFunction> struct DeleterTraits;
template<typename TReturn, typename TParam> struct DeleterTraits<TReturn (*)(TParam)>
{
	using TParameter = TParam;
};

export template<typename T, auto Delete = defaultDelete<T>> class Unique
{
	template<typename, auto> friend class Unique;

	using TDeleterParameter = typename DeleterTraits<decltype(Delete)>::TParameter;
	using THandle			= std::conditional_t<std::convertible_to<T*, TDeleterParameter>, T*, TDeleterParameter>;

public:
	template<typename... Ts> static Unique from(Ts&&... ts)
	{
		return Unique(new T(std::forward<Ts>(ts)...));
	}

	Unique() noexcept = default;

	Unique(THandle handle) noexcept : handle(handle)
	{}

	Unique(Unique&& that) noexcept : Unique()
	{
		swap(that);
	}

	template<typename TOther, auto DeleteOther>
	Unique(Unique<TOther, DeleteOther>&& that) noexcept : handle(std::exchange(that.handle, nullptr))
	{}

	~Unique()
	{
		deleteHandle();
	}

	Unique& operator=(Unique&& that) noexcept
	{
		swap(that);
		return *this;
	}

	template<typename TOther, auto DeleteOther> Unique& operator=(Unique<TOther, DeleteOther>&& that) noexcept
	{
		deleteHandle();
		handle = std::exchange(that.handle, nullptr);
		return *this;
	}

	auto& operator*() const noexcept
	{
		return *handle;
	}

	THandle operator->() const noexcept
	{
		return handle;
	}

	auto operator->*(auto memberPointer) const noexcept
	{
		return [this, memberPointer](auto&&... args) {
			return (this->handle->*memberPointer)(std::forward<decltype(args)>(args)...);
		};
	}

	THandle* operator&() noexcept
	{
		return &this->handle;
	}

	THandle const* operator&() const noexcept
	{
		return &this->handle;
	}

	auto operator<=>(auto that) const noexcept
	{
		return handle <=> that;
	}

	operator THandle() const noexcept
	{
		return handle;
	}

	operator bool() const noexcept
	{
		return handle;
	}

	THandle get() const noexcept
	{
		return handle;
	}

	[[nodiscard]] THandle release() noexcept
	{
		return std::exchange(handle, nullptr);
	}

	void reset(THandle resetValue = THandle()) noexcept
	{
		deleteHandle();
		handle = resetValue;
	}

	void swap(Unique& that) noexcept
	{
		std::swap(handle, that.handle);
	}

	friend void swap(Unique& left, Unique& right) noexcept
	{
		left.swap(right);
	}

protected:
	THandle handle {};

	void deleteHandle()
	{
		Delete(handle);
	}
};
