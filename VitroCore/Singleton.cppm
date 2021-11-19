module;
#include "VitroCore/Macros.hpp"
export module vt.Core.Singleton;

namespace vt
{
	export template<typename T> class Singleton
	{
	public:
		static T& get()
		{
			VT_ASSERT(singleton, "Accessing a destroyed singleton.");
			return *singleton;
		}

	protected:
		Singleton()
		{
			VT_ENSURE(!singleton, "This type can only be instantiated once.");
			singleton = static_cast<T*>(this);
		}

		Singleton(Singleton&&) noexcept
		{
			singleton = static_cast<T*>(this);
		}

		~Singleton()
		{
			singleton = nullptr;
		}

		Singleton& operator=(Singleton&&) noexcept
		{
			singleton = static_cast<T*>(this);
			return *this;
		}

	private:
		// TODO: maybe change to in-place buffer for more direct access, i.e. no dereference necessary? Measure first.
		static inline T* singleton;
	};
}
