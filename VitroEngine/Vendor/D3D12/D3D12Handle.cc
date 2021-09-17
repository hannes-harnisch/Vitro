﻿module;
#include "D3D12API.hh"

#include <memory>
export module vt.D3D12.Handle;

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
}