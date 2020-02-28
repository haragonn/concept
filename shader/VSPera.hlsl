struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

VS_OUT main(float4 pos : POSITION, float2 tex : TEXCOORD)
{
	VS_OUT output;

	output.pos = pos;
	output.tex = tex;

	return output;
}