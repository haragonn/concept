#ifndef INCLUDE_EIDOS_OBJECTMANAGER_H
#define INCLUDE_EIDOS_OBJECTMANAGER_H

#include "../../../idea/h/Shader/Shader.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

class ObjectManager{
public:
	static const int CUBE_VERTEX_NUM = 24;
	static const int CUBE_INDEX_NUM = 36;

	static ObjectManager& Instance()	// 唯一のインスタンスを返す
	{
		static ObjectManager s_Instance;
		return s_Instance;
	}

	bool Init();
	void UnInit();

	ID3D11VertexShader* GetVertexShederPtr()const{ return pVertexShader_; }
	ID3D11VertexShader* GetShadowVertexShederPtr()const{ return pShadowVertexShader_; }
	ID3D11VertexShader* GetPmdVertexShederPtr()const{ return pPmdVertexShader_; }

	ID3D11InputLayout* GetInputLayoutPtr()const{ return pVertexLayout_; }
	ID3D11InputLayout* GetShadowInputLayoutPtr()const{ return pShadowVertexLayout_; }
	ID3D11InputLayout* GetPmdInputLayoutPtr()const{ return pPmdVertexLayout_; }

	ID3D11PixelShader* GetPixelShederDefaultPtr()const{ return pixelShaderDefault_.GetPixelShaderPtr(); }
	ID3D11PixelShader* GetPixelShaderShadowPtr()const{ return pixelShaderShadow_.GetPixelShaderPtr(); }
	ID3D11PixelShader* GetPixelShederTexturePtr()const{ return pixelShaderTexture_.GetPixelShaderPtr(); }
	ID3D11PixelShader* GetPixelShaderTextureShadowPtr()const{ return pixelShaderTextureShadow_.GetPixelShaderPtr(); }

	ID3D11Buffer* GetCubeVertexBufferPtr(){ return pCubeVertexBuffer_; }
	ID3D11Buffer* GetCubeIndexBufferPtr(){ return pCubeIndexBuffer_; }

	ID3D11Buffer* GetBillboardVertexBufferPtr(){ return pBillboardVertexBuffer_; }
	ID3D11Buffer* GetBillboardIndexBufferPtr(){ return pBillboardIndexBuffer_; }

	ID3D11Buffer* GetConstBufferPtr()const{ return pConstBuffer_; }
	ID3D11Buffer* GetShadowConstBufferPtr()const{ return pShadowConstBuffer_; }
	ID3D11Buffer* GetPmdConstBufferPtr()const{ return pPmdConstBuffer_; }

	void SetLight(float axisX, float axisY, float axisZ);
	DirectX::XMVECTOR& GetLight(){ return light_; }

public:
	ID3D11VertexShader* pVertexShader_;
	ID3D11VertexShader* pShadowVertexShader_;
	ID3D11VertexShader* pPmdVertexShader_;

	PixelShader pixelShaderDefault_;
	PixelShader pixelShaderShadow_;
	PixelShader pixelShaderTexture_;
	PixelShader pixelShaderTextureShadow_;

	ID3D11InputLayout* pVertexLayout_;
	ID3D11InputLayout* pShadowVertexLayout_;
	ID3D11InputLayout* pPmdVertexLayout_;

	ID3D11Buffer* pCubeVertexBuffer_;
	ID3D11Buffer* pCubeIndexBuffer_;
	ID3D11Buffer* pBillboardVertexBuffer_;
	ID3D11Buffer* pBillboardIndexBuffer_;

	ID3D11Buffer* pConstBuffer_;
	ID3D11Buffer* pPmdConstBuffer_;
	ID3D11Buffer* pShadowConstBuffer_;

	DirectX::XMVECTOR light_;

	ObjectManager();				// コンストラクタ
	~ObjectManager(){ UnInit(); }	// デストラクタ

	// コピーコンストラクタの禁止
	ObjectManager(const ObjectManager& src){}
	ObjectManager& operator=(const ObjectManager& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_OBJECTMANAGER_H