cbuffer PushConstants : register(b0)
{
	float4 triangle_color;
};

struct VertexOut
{
	float4 position : SV_POSITION;
};

VertexOut main(uint index : SV_VertexID)
{
	VertexOut vertex_out;
	switch(index)
	{
		case 0: vertex_out.position = float4(0, 0.5, 0.5, 1); break;
		case 1: vertex_out.position = float4(0.5, -0.5, 0.5, 1); break;
		case 2: vertex_out.position = float4(-0.5, -0.5, 0.5, 1); break;
	}
	return vertex_out;
}
