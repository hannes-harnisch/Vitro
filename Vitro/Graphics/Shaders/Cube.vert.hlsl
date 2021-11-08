#include "Vitro.hlsli"

struct PushConstants
{
	float4	 triangle_color;
	float4x4 mvp;
};
PUSH_CONST(PushConstants, constants);

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
	VertexOut vertex_out;

	vertex_out.position = mul(constants.mvp, vertex_in.position);
	vertex_out.color	= vertex_in.color;

	return vertex_out;
}
