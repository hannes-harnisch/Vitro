module;
#include "Direct3D.API.hh"
#include "Trace/Assert.hh"

#include <array>
export module Vitro.Direct3D.DriverContext;

import Vitro.Direct3D.Unique;

class GraphicsSystem;

namespace Direct3D
{
	export class DriverContext final
	{
		friend ::GraphicsSystem;

	private:
		constexpr static D3D_FEATURE_LEVEL Level = D3D_FEATURE_LEVEL_11_0;

#if VT_DEBUG
		Unique<ID3D12Debug> debug;
#endif

		DriverContext()
		{
			createDebugInterface();
		}

		void createDebugInterface()
		{
#if VT_DEBUG
			vtEnsureResult(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)), "Cannot get debug interface.");
			debug->EnableDebugLayer();
#endif
		}
	};
}
