Texture2D<float4> myTexture : register(t0);
Texture2D<float> shadowTexture : register(t1);
SamplerState mySampler : register(s0);

struct VS_OUT
{
	float4 pos : SV_POSITION; // �ˉe�ϊ����W
	float2 tex : TEXCOORD0;
	float4 ZCalcTex : TEXCOORD1; // Z�l�Z�o�p�e�N�X�`��
	float4 col : COLOR0; // �f�B�t���[�Y�F
};
float4 main(VS_OUT input) : SV_TARGET
{
	// ���C�g�ڐ��ɂ��Z�l�̍ĎZ�o
	float3 ZValue = input.ZCalcTex.xyz / input.ZCalcTex.w;

	// �ˉe��Ԃ�XY���W���e�N�X�`�����W�ɕϊ�
	float2 TransTexCoord = (ZValue.xy + float2(1, -1)) * float2(0.5f, -0.5f);

	if(TransTexCoord.x > 0.01f
		&& TransTexCoord.x < 0.99f
		&& TransTexCoord.y > 0.01f
		&& TransTexCoord.y < 0.99f){
		// ���A��Z�l���o
		float SM_Z = shadowTexture.Sample(mySampler, TransTexCoord);

		// �Z�o�_���V���h�E�}�b�v��Z�l�����傫����Ήe�Ɣ��f
		if(SM_Z <= ZValue.z - 0.005f){
			input.col.rgb = input.col.rgb * 0.4f;
		}
	}

	clip(myTexture.Sample(mySampler, input.tex).w - 1.175494351e-38f);
	return myTexture.Sample(mySampler, input.tex) * input.col;
}