cbuffer ConstantBuffer : register(b1)
{
	float4x4 world;
	float4x4 view;
	float4x4 projection;
	float4 color;
	float4 light;
}
cbuffer ConstantBuffer : register(b2)
{
	float4x4 boneWorld[512];
}

struct VS_IN {
	float4	pos		: POSITION;
	float4	nor		: NORMAL;
	float2	tex		: TEXCOORD0;
	float4	weight	: BLENDWEIGHT; 
	uint4	idx		: BLENDINDICES;
};

struct VS_OUT {
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
};

VS_OUT main(VS_IN input) 
{
	VS_OUT output;

	float4 pos;
	float4 normalHead;
	float3 normal;
	float col;

	float w[3] = (float[3])input.weight;
	float4x4 comb = boneWorld[input.idx[0]] * w[0] + boneWorld[input.idx[1]] * w[1];

	pos = mul(comb, float4(input.pos.xyz, 1.0));
	normalHead = mul(float4(input.pos.xyz + input.nor.xyz, 1.0), comb);
	normal = normalize(input.nor.xyz);

	output.pos = mul(pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	col = saturate(dot(normal, normalize(light.xyz)));
	col = (col * 0.5f + 0.5f);
	output.col = float4(col, col, col, 1.0f) * color;

	output.tex = input.tex;

	return output;
}