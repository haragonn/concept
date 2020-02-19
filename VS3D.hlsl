cbuffer ConstantBuffer : register(b1)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
	float4 color;
	float4 light;
}

struct VS_IN
{
	float4 pos : POSITION0;
	float4 nor : NORMAL0;
	//float4 col : COLOR0;
	float2 tex : TEXCOORD0;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 tex : TEXCOORD0;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output;
	float4x4 w = world;
	float3 nor;
	float  col;

	output.pos = mul(input.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	
	w._41 = 0.0f;
	w._42 = 0.0f;
	w._43 = 0.0f;

	nor = mul(normalize(input.nor), w).xyz;
	//nor = normalize(nor);

	col = saturate(dot(nor, normalize(light.xyz)));
	col = (col * 0.5f + 0.5f);

	output.col = float4(col, col, col, 1.0f) * color;
	output.tex = input.tex;

	return output;
}