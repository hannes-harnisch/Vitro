#pragma once

#include <d3dcommon.h>

namespace Vitro::DirectX
{
	enum class VertexTopology : unsigned char
	{
		PointList	  = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,
		LineList	  = D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		LineStrip	  = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		TriangleList  = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		TriangleStrip = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
	};
}
