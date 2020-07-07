#pragma once

#if VTR_API_DIRECTX
	#include "Vitro/API/DirectX/VertexTopology.h"
#endif

namespace Vitro
{
#if VTR_API_DIRECTX
	typedef DirectX::VertexTopology VertexTopology;
#else
	#error Unsupported graphics API.
#endif
}
