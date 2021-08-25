module;
#include "Core/Macros.hh"
export module vt.Core.Singleton;

namespace vt
{
	export template<typename T> class Singleton
	{
	public:
		static T& get()
		{
			return *instance;
		}

		Singleton(Singleton const&) = delete;
		Singleton& operator=(Singleton const&) = delete;

	protected:
		static inline T* instance;

		Singleton()
		{
			VT_ENSURE(!instance, "This type can only be instantiated once.");
			instance = static_cast<T*>(this);
		}
	};
}
