Texture2D Ground;
SamplerState Sampler;

float4 main(float4 position : SV_POSITION, float2 tex0 : TEXCOORD0) : SV_TARGET
{
	return Ground.Sample(Sampler, tex0);
}
