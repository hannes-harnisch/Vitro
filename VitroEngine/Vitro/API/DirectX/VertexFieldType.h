#pragma once

#include <dxgiformat.h>

namespace Vitro::DirectX
{
	enum class VertexFieldType : unsigned char
	{
		None   = DXGI_FORMAT_UNKNOWN,
		Float  = DXGI_FORMAT_R32_FLOAT,
		Float2 = DXGI_FORMAT_R32G32_FLOAT,
		Float3 = DXGI_FORMAT_R32G32B32_FLOAT,
		Float4 = DXGI_FORMAT_R32G32B32A32_FLOAT,
		Int	   = DXGI_FORMAT_R32_SINT,
		Int2   = DXGI_FORMAT_R32G32_SINT,
		Int3   = DXGI_FORMAT_R32G32B32_SINT,
		Int4   = DXGI_FORMAT_R32G32B32A32_SINT,
		Bool   = DXGI_FORMAT_R8_SINT
	};
}
