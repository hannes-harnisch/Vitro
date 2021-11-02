module;
#include "Core/Macros.hpp"
#include "D3D12API.hpp"
export module vt.Graphics.D3D12.Handle;

namespace vt::d3d12
{
	template<typename T> struct ComDeleter
	{
		void operator()(T* com_interface) const
		{
			com_interface->Release();
		}
	};
	export template<typename T> using ComUnique = std::unique_ptr<T, ComDeleter<T>>;

	export using D3D12Adapter			= ComUnique<IDXGIAdapter1>;
	export using D3D12CommandListHandle = ID3D12CommandList*;

	export struct D3D12SyncToken
	{
		ID3D12Fence* fence		 = nullptr;
		uint64_t	 fence_value = 0;
	};
}
