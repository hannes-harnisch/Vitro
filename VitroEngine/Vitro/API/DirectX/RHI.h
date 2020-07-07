#pragma once

#include "Vitro/API/DirectX/Scope.h"

#include <d3d11_4.h>

namespace Vitro::DirectX
{
	class RHI final
	{
	public:
		static inline Scope<ID3D11Device5> Device;
		static inline Scope<ID3D11DeviceContext4> Context;
#if VTR_DEBUG
		static inline Scope<ID3D11Debug> DebugLayer;
#endif

		static void Initialize();
	};
}
