module;
#include "D3D12.API.hh"
export module Vitro.D3D12.ComUnique;

import Vitro.Core.Unique;

namespace vt::d3d12
{
	void deleteInterface(IUnknown* const comInterface)
	{
		comInterface->Release();
	}

	export template<typename TInterface> class ComUnique : public Unique<TInterface, deleteInterface>
	{
	public:
		using Unique<TInterface, deleteInterface>::Unique;

		template<typename TOtherInterface> HRESULT queryFor(TOtherInterface** const targetInterface)
		{
			return this->get()->QueryInterface(targetInterface);
		}
	};
}
