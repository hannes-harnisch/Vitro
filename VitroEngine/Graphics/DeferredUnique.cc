module;
#include <memory>
export module Vitro.Graphics.DeferredUnique;

import Vitro.Graphics.DeferredDeleter;

namespace vt
{
	template<typename T> struct DeferredDeleterSubmitter
	{
		using pointer = T;

		void operator()(T handle) const
		{
			DeferredDeleter::get()->submit(handle);
		}
	};

	export template<typename T> using DeferredUnique = std::unique_ptr<T, DeferredDeleterSubmitter<T>>;
}
