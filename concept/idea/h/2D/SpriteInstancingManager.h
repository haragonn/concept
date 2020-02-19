#ifndef INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H
#define INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H

#include <d3d11.h>
#include <directxmath.h>

struct PerInstanceData
{
	DirectX::XMFLOAT4 pos;
};

class SpriteInstancingManager{
public:
	static const int INSTANCE_MAX = 65536;

	static SpriteInstancingManager& Instance()	// 唯一のインスタンスを返す
	{
		static SpriteInstancingManager s_Instance;
		return s_Instance;
	}
	bool Init();
	void UnInit();
	ID3D11VertexShader* GetVertexShederPtr()const{ return pVertexShader_; }
	//ID3D11InputLayout* GetInputLayoutPtr()const{ return pVertexLayout_; }
	//ID3D11Buffer* GetRectVertexBufferPtr()const{ return pRectVertexBuffer_; }
	//ID3D11Buffer* GetConstBufferPtr()const{ return pConstBuffer_; }
	ID3D11Buffer* GetInstanceDataBufferPtr()const{ return pInstanceDataBuffer_; }
	ID3D11ShaderResourceView* GetTransformShaderResourceViewPtr()const{ return pTransformShaderResourceView_; }

private:
	ID3D11VertexShader* pVertexShader_;
	//ID3D11InputLayout* pVertexLayout_;
	//ID3D11Buffer* pRectVertexBuffer_;
	//ID3D11Buffer* pConstBuffer_;
	ID3D11Buffer* pInstanceDataBuffer_;
	ID3D11ShaderResourceView* pTransformShaderResourceView_;

	SpriteInstancingManager();					// コンストラクタ
	~SpriteInstancingManager(){ UnInit(); }	// デストラクタ
										// コピーコンストラクタの禁止
	SpriteInstancingManager(const SpriteInstancingManager& src){}
	SpriteInstancingManager& operator=(const SpriteInstancingManager& src){}
};


#endif	// #ifndef INCLUDE_IDEA_SPRITEINSTANCINGMANAGER_H