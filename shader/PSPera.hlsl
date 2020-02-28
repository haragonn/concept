Texture2D<float4> myTexture : register(t0);
SamplerState mySampler : register(s0);

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
}; 


float4 boke(VS_OUT input) : SV_TARGET
{
	float w, h, level;
	myTexture.GetDimensions(0, w, h, level);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 ret = float4(0, 0, 0, 0);

	//今のピクセルを中心に縦横5つずつになるよう加算する
	//最上段
	ret += myTexture.Sample(mySampler, input.tex + float2(-2 * dx, 2 * dy)) * 1;
	ret += myTexture.Sample(mySampler, input.tex + float2(-1 * dx, 2 * dy)) * 4;
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, 2 * dy)) * 6;
	ret += myTexture.Sample(mySampler, input.tex + float2(1 * dx, 2 * dy)) * 4;
	ret += myTexture.Sample(mySampler, input.tex + float2(2 * dx, 2 * dy)) * 1;
	//ひとつ上段
	ret += myTexture.Sample(mySampler, input.tex + float2(-2 * dx, 1 * dy)) * 4;
	ret += myTexture.Sample(mySampler, input.tex + float2(-1 * dx, 1 * dy)) * 16;
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, 1 * dy)) * 24;
	ret += myTexture.Sample(mySampler, input.tex + float2(1 * dx, 1 * dy)) * 16;
	ret += myTexture.Sample(mySampler, input.tex + float2(2 * dx, 1 * dy)) * 4;
	//中心列
	ret += myTexture.Sample(mySampler, input.tex + float2(-2 * dx, 0 * dy)) * 6;
	ret += myTexture.Sample(mySampler, input.tex + float2(-1 * dx, 0 * dy)) * 24;
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, 0 * dy)) * 36;
	ret += myTexture.Sample(mySampler, input.tex + float2(1 * dx, 0 * dy)) * 24;
	ret += myTexture.Sample(mySampler, input.tex + float2(2 * dx, 0 * dy)) * 6;
	//一つ下段
	ret += myTexture.Sample(mySampler, input.tex + float2(-2 * dx, -1 * dy)) * 4;
	ret += myTexture.Sample(mySampler, input.tex + float2(-1 * dx, -1 * dy)) * 16;
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, -1 * dy)) * 24;
	ret += myTexture.Sample(mySampler, input.tex + float2(1 * dx, -1 * dy)) * 16;
	ret += myTexture.Sample(mySampler, input.tex + float2(2 * dx, -1 * dy)) * 4;
	//最下段
	ret += myTexture.Sample(mySampler, input.tex + float2(-2 * dx, -2 * dy)) * 1;
	ret += myTexture.Sample(mySampler, input.tex + float2(-1 * dx, -2 * dy)) * 4;
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, -2 * dy)) * 6;
	ret += myTexture.Sample(mySampler, input.tex + float2(1 * dx, -2 * dy)) * 4;
	ret += myTexture.Sample(mySampler, input.tex + float2(2 * dx, -2 * dy)) * 1;

	ret /= 256;

	ret.a = 1.0f;

	return ret;
}

float4 Emboss(VS_OUT input) : SV_TARGET
{
	float w, h, level;
	myTexture.GetDimensions(0, w, h, level);
	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 ret = float4(0, 0, 0, 0);


	//今のピクセルを中心に縦横5つずつになるよう加算する
	//最上段
	ret += myTexture.Sample(mySampler, input.tex + float2(-2 * dx, 2 * dy)) * 2;
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, 2 * dy));
	ret += myTexture.Sample(mySampler, input.tex + float2(2 * dx, 2 * dy)) * 0;
	//中心列
	ret += myTexture.Sample(mySampler, input.tex + float2(-2 * dx, 0 * dy));
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, 0 * dy));
	ret += myTexture.Sample(mySampler, input.tex + float2(2 * dx, 0 * dy)) * -1;
	//最下段
	ret += myTexture.Sample(mySampler, input.tex + float2(-2 * dx, -2 * dy)) * 0;
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, -2 * dy)) * -1;
	ret += myTexture.Sample(mySampler, input.tex + float2(2 * dx, -2 * dy)) * -2;

	ret.a = 1.0f;

	return ret;
}

float4 Monochrome(float4 input) : SV_TARGET
{
	float ret = dot(input.rgb, float3(0.2126f, 0.7152f, 0.0722f));
	return float4(ret, ret, ret, 1);
}

float4 Sharpness(VS_OUT input) : SV_TARGET
{
	float w, h, level;
	myTexture.GetDimensions(0, w, h, level);

	float dx = 1.0f / w;
	float dy = 1.0f / h;
	float4 ret = float4(0, 0, 0, 0);

	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, -2 * dy)) * -1;
	ret += myTexture.Sample(mySampler, input.tex + float2(-2 * dx, 0 * dy)) * -1;
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, 0 * dy)) * 4;
	ret += myTexture.Sample(mySampler, input.tex + float2(0 * dx, 2 * dy)) * -1;
	ret += myTexture.Sample(mySampler, input.tex + float2(2 * dx, 0 * dy)) * -1;

	ret.a = 1.0f;

	return ret;
}

float4 main(VS_OUT input) : SV_TARGET
{
	float4 col = myTexture.Sample(mySampler, input.tex);

	return Monochrome(col);
}