#include "Vitro.hlsli"

struct PushConstants
{
	float4 triangle_color;
};

PushConstants(PushConstants, constants);

struct VertexOut
{
	float4 position : SV_POSITION;
};

float4 main(VertexOut position) : SV_TARGET
{
	return constants.triangle_color;
}
