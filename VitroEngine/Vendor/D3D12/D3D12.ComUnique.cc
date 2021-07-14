module;
#include "D3D12.API.hh"
export module Vitro.D3D12.ComUnique;

import Vitro.Core.Unique;

namespace vt::d3d12
{
	void deleteInterface(IUnknown* const ptr)
	{
		ptr->Release();
	}

	export template<typename TInterface> using ComUnique = Unique<TInterface, deleteInterface>;
}
