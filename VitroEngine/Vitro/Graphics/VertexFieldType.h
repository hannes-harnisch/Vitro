#pragma once

#if VTR_API_DIRECTX
	#include "Vitro/API/DirectX/VertexFieldType.h"
#endif

namespace Vitro
{
#if VTR_API_DIRECTX
	typedef DirectX::VertexFieldType VertexFieldType;
#else
	#error Unsupported graphics API.
#endif
}
