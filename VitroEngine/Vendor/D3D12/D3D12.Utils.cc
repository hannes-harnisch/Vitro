module;
#include <memory>
export module Vitro.D3D12.Utils;

namespace vt::d3d12
{
	template<typename T> struct InterfaceDeleter
	{
		void operator()(T* ptr)
		{
			ptr->Release();
		}
	};

	export template<typename T> using ComUnique = std::unique_ptr<T, InterfaceDeleter<T>>;
}
