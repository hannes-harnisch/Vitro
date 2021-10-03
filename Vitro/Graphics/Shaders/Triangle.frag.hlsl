cbuffer PushConstants : register(b0)
{
	float4 triangle_color;
};

struct VertexOut
{
	float4 position : SV_POSITION;
};

float4 main(VertexOut position) : SV_TARGET
{
	return triangle_color;
}
