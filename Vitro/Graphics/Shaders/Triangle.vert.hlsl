#include "Vitro.hlsli"

struct VertexIn
{
	float4 position : POSITION0;
};

struct VertexOut
{
	float4 position : SV_POSITION;
};

VertexOut main(VertexIn vertex_in)
{
	return vertex_in;
}
