#include "Vitro.hlsli"

struct VertexIn
{
	float4 position : POSITION0;
	float4 color : COLOR;
};

struct VertexOut
{
	float4 position : SV_Position;
	float4 color : COLOR;
};

VertexOut main(VertexIn vertex_in)
{
	return vertex_in;
}
