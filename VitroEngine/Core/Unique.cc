module;
#include <memory>
export module Vitro.Core.Unique;

namespace vt
{
	template<typename> struct DeleterTraits;
	template<typename TReturn, typename TParam> struct DeleterTraits<TReturn (*)(TParam)>
	{
		using Target = TParam;
	};

	template<typename T, auto Delete> struct ProxyDeleter
	{
		using pointer = typename DeleterTraits<decltype(Delete)>::Target;

		void operator()(pointer ptr) const
		{
			Delete(ptr);
		}
	};

	// Making std::unique_ptr easier to use with custom deleters.
	export template<typename T, auto Delete> using Unique = std::unique_ptr<T, ProxyDeleter<T, Delete>>;
}
