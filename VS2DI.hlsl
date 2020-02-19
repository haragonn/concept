cbuffer constBuffer : register(b0)
{
	float4x4 proj;
}
struct PerInstanceData
{
	float4 pos;
};

StructuredBuffer<PerInstanceData>	perInstanceData :register(t1);

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

VS_OUT main(VS_IN input, uint instanceID : SV_InstanceID)
{
	VS_OUT output;

	output.pos = mul(float4(input.pos.x + perInstanceData[instanceID].pos.x, input.pos.y + perInstanceData[instanceID].pos.y, 0.0f, 1.0f), proj);
	output.col = input.col;
	output.tex  = input.tex;

	return output;
}