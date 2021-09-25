module;
#include "D3D12API.hpp"

#include <memory>
export module vt.Graphics.D3D12.Handle;

namespace vt::d3d12
{
	template<typename T> struct ComDeleter
	{
		void operator()(T* ptr)
		{
			ptr->Release();
		}
	};
	export template<typename T> using ComUnique = std::unique_ptr<T, ComDeleter<T>>;

	export using D3D12Adapter			= ComUnique<IDXGIAdapter1>;
	export using D3D12CommandListHandle = ID3D12CommandList*;

	export struct D3D12SyncValue
	{
		ID3D12Fence* wait_fence		  = nullptr;
		uint64_t	 wait_fence_value = 0;
	};
}
