module;
#include "Direct3D.API.hh"
export module Vitro.Direct3D.Unique;

import Vitro.Core.Unique;

namespace Direct3D
{
	void deleteInterface(IUnknown* comObject)
	{
		if(comObject)
			comObject->Release();
	}

	export template<typename TInterface> class Unique : public ::Unique<TInterface, deleteInterface>
	{
	public:
		TInterface** operator&()
		{
			this->deleteHandle();
			return &this->handle;
		}

		TInterface* const* operator&() const
		{
			return &this->handle;
		}

		template<typename TOtherInterface> HRESULT queryFor(TOtherInterface** targetInterface)
		{
			return this->handle->QueryInterface(targetInterface);
		}
	};
}
