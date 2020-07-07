cbuffer CameraUniforms
{
	float4x4 MVP;
};

struct VOut
{
	float4 position : SV_POSITION;
	float2 tex0 : TEXCOORD0;
};

VOut main(float4 position : POSITION, float2 tex0 : TEXCOORD0)
{
	VOut output;
	output.position = mul(MVP, position);
	output.tex0 = tex0;
	return output;
}
