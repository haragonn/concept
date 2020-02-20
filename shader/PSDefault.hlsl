struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float2 tex : TEXCOORD0;
};

float4 main(VS_OUT input) : SV_TARGET
{
	return input.col;
}