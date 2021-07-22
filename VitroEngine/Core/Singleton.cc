module;
#include "Core/Intrinsics.hh"
#include "Trace/Assert.hh"
export module Vitro.Core.Singleton;

extern "C" VT_DLLEXPORT void setEngineInterface(class Engine*);

namespace vt
{
	export template<typename T> class Singleton
	{
		friend void ::setEngineInterface(Engine*);

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
			vtEnsure(!instance, "This type can only be instantiated once.");
			instance = static_cast<T*>(this);
		}
	};
}
