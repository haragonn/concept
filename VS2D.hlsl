cbuffer constBuffer : register(b0)
{
	float4x4 proj;
}

struct VS_IN
{
	float4 pos : POSITION0;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output;

	output.pos = mul(float4(input.pos.x, input.pos.y, 0.0f, 1.0f), proj);
	output.col = input.col;
	output.tex  = input.tex;

	return output;
}