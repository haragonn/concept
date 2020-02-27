#ifndef INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H
#define INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

struct PerInstanceData
{
	DirectX::XMFLOAT4 pos;
};

class SpriteInstancingManager{
public:
	static const int INSTANCE_MAX = 65536;

	static SpriteInstancingManager& Instance()	// �B��̃C���X�^���X��Ԃ�
	{
		static SpriteInstancingManager s_Instance;
		return s_Instance;
	}

	bool Init();
	void UnInit();

	ID3D11VertexShader* GetVertexShederPtr()const{ return pVertexShader_; }

	ID3D11Buffer* GetInstanceDataBufferPtr()const{ return pInstanceDataBuffer_; }

	ID3D11ShaderResourceView* GetTransformShaderResourceViewPtr()const{ return pTransformShaderResourceView_; }

private:
	ID3D11VertexShader* pVertexShader_;

	ID3D11Buffer* pInstanceDataBuffer_;

	ID3D11ShaderResourceView* pTransformShaderResourceView_;

	SpriteInstancingManager();				// �R���X�g���N�^
	~SpriteInstancingManager(){ UnInit(); }	// �f�X�g���N�^

	// �R�s�[�R���X�g���N�^�̋֎~
	SpriteInstancingManager(const SpriteInstancingManager& src){}
	SpriteInstancingManager& operator=(const SpriteInstancingManager& src){}
};


#endif	// #ifndef INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H