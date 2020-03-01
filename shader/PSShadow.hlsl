Texture2D<float> shadowTexture : register(t1);
SamplerState mySampler : register(s0);

struct VS_OUT
{
    float4 pos : SV_POSITION; // 射影変換座標
    float2 tex : TEXCOORD0;
    float4 ZCalcTex : TEXCOORD1; // Z値算出用テクスチャ
    float4 col : COLOR0; // ディフューズ色
};
float4 main(VS_OUT input) : SV_TARGET
{
    // ライト目線によるZ値の再算出
    float3 ZValue = input.ZCalcTex.xyz / input.ZCalcTex.w;

    // 射影空間のXY座標をテクスチャ座標に変換
    float2 TransTexCoord = (ZValue.xy + float2(1, -1)) * float2(0.5f, -0.5f);

    if(TransTexCoord.x >= 0.0f
        && TransTexCoord.x <= 1.0f
        && TransTexCoord.y >= 0.0f
        && TransTexCoord.y <= 1.0f){
        // リアルZ値抽出
        float SM_Z = shadowTexture.Sample(mySampler, TransTexCoord);

        // 算出点がシャドウマップのZ値よりも大きければ影と判断
        if(SM_Z <= ZValue.z - 0.005f){
            input.col.rgb = input.col.rgb * 0.5f;
        }
    }

    return input.col;
}