export module Vitro.D3D12.Resource;

namespace D3D12
{
	export template<typename T> struct Resource
	{
		void destroy()
		{
			if(static_cast<T*>(this)->handle)
				static_cast<T*>(this)->handle->Release();
		}
	};
}
