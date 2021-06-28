module;
#include "D3D12.API.hh"
export module Vitro.D3D12.Unique;

import Vitro.Core.Unique;

namespace vt::d3d12
{
	void deleteInterface(IUnknown* const comObject)
	{
		if(comObject)
			comObject->Release();
	}

	export template<typename TInterface> class Unique : public vt::Unique<TInterface, deleteInterface>
	{
	public:
		using vt::Unique<TInterface, deleteInterface>::Unique;

		template<typename TOtherInterface> HRESULT queryFor(TOtherInterface** const targetInterface)
		{
			return this->handle->QueryInterface(targetInterface);
		}
	};
}
