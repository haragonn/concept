Texture2D<float4> myTexture : register(t0);
SamplerState mySampler : register(s0);

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float4 col = myTexture.Sample(mySampler, input.tex);

	return float4(col.rgb, 1.0f);
}