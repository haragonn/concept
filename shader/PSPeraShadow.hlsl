Texture2D<float> depthTex : register(t1);
SamplerState mySampler : register(s0);

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
	float dep = pow(abs(depthTex.Sample(mySampler, input.tex)), 10);

	return float4(dep, dep, dep, 1.0f);
}