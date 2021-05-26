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

	Unique() = default;

	Unique(THandle handle) : handle(handle)
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

	auto& operator*() const
	{
		return *handle;
	}

	THandle operator->() const
	{
		return handle;
	}

	auto operator<=>(auto that) const
	{
		return handle <=> that;
	}

	operator THandle() const
	{
		return handle;
	}

	operator bool() const
	{
		return handle;
	}

	THandle get() const
	{
		return handle;
	}

	[[nodiscard]] THandle release()
	{
		return std::exchange(handle, nullptr);
	}

	void reset(THandle resetValue = THandle())
	{
		deleteHandle();
		handle = resetValue;
	}

	void swap(Unique& that)
	{
		std::swap(handle, that.handle);
	}

	friend void swap(Unique& left, Unique& right)
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
