Texture2D<float4> myTexture : register(t0);
SamplerState mySampler : register(s0);

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
	clip(myTexture.Sample(mySampler, input.tex).w - 1.175494351e-38f);
	return myTexture.Sample(mySampler, input.tex) * input.col;
}