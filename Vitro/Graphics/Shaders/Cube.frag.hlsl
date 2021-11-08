#include "Vitro.hlsli"

struct PushConstants
{
	float4	 triangle_color;
	float4x4 mvp;
};
PUSH_CONST(PushConstants, constants);

struct VertexOut
{
	float4 position : SV_Position;
	float4 color : COLOR;
};

float4 main(VertexOut vertex_out) : SV_Target
{
	return vertex_out.color * constants.triangle_color;
}
