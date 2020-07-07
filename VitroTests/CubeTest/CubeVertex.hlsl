cbuffer CameraUniforms
{
	float4x4 MVP;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VOut main(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;
	output.position = mul(MVP, position);
	output.color = color;
	return output;
}
