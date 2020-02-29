cbuffer ConstantBuffer : register(b3)
{
    float4x4 world;
    float4x4 view;
    float4x4 projection;
    float4 color;
    float4 light;
    float4x4 matLightView;               // ���C�g�r���[�ϊ��s��
    float4x4 matLightProj;               // �ˉe�ϊ��s��
}

struct VS_IN
{
    float4 pos : POSITION0;
    float4 nor : NORMAL0;
    float4 col : COLOR0;
    float2 tex : TEXCOORD0;
};

struct VS_OUT
{
    float4 pos : SV_POSITION; // �ˉe�ϊ����W
    float2 tex : TEXCOORD0;
    float4 ZCalcTex : TEXCOORD1; // Z�l�Z�o�p�e�N�X�`��
    float4 col : COLOR0; // �f�B�t���[�Y�F
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;

    float4x4 w = world;
    float3 nor;
    float  col;
    float4x4 mat;

    mat = mul(world, view);
    mat = mul(mat, projection);
    output.pos = mul(input.pos, mat);

    w._41 = 0.0f;
    w._42 = 0.0f;
    w._43 = 0.0f;

    nor = mul(normalize(input.nor), w).xyz;
    nor = normalize(nor);

    col = saturate(dot(nor, normalize(light.xyz)));
    col = (col * 0.5f + 0.5f);

    output.col = float4(col, col, col, 1.0f) * color;
    output.tex = input.tex;

    // ���C�g�̖ڐ��ɂ�郏�[���h�r���[�ˉe�ϊ�������
    mat = mul(world, matLightView);
    mat = mul(mat, matLightProj);
    output.ZCalcTex = mul(input.pos, mat);

    return output;
}