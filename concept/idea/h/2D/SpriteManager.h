#ifndef INCLUDE_IDEA_SPRITEMANAGER_H
#define INCLUDE_IDEA_SPRITEMANAGER_H

#include "../../../idea/h/Shader/Shader.h"

struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;

class SpriteManager{
public:
	static const int RECT_VERTEX_NUM = 4;
	static const int CIRCLE_VERTEX_NUM = 256;

	static SpriteManager& Instance()	// 唯一のインスタンスを返す
	{
		static SpriteManager s_Instance;
		return s_Instance;
	}

	bool Init();
	void UnInit();

	ID3D11Buffer* GetRectVertexBufferPtr()const{ return pRectVertexBuffer_; }

	ID3D11Buffer* GetCircleVertexBufferPtr()const{ return pCircleVertexBuffer_; }
	ID3D11Buffer* GetCircleIndexBufferPtr()const{ return pCircleIndexBuffer_; }

	ID3D11InputLayout* GetInputLayoutPtr()const{ return pVertexLayout_; }
	ID3D11Buffer* GetConstBufferPtr()const{ return pConstBuffer_; }
	ID3D11VertexShader* GetVertexShederPtr()const{ return pVertexShader_; }

	ID3D11PixelShader* GetPixelShederDefaultPtr()const{ return pixelShaderDefault_.GetPixelShaderPtr(); }
	ID3D11PixelShader* GetPixelShederTexturePtr()const{ return pixelShaderTexture_.GetPixelShaderPtr(); }


private:
	ID3D11Buffer* pRectVertexBuffer_;

	ID3D11Buffer* pCircleVertexBuffer_;
	ID3D11Buffer* pCircleIndexBuffer_;

	ID3D11InputLayout* pVertexLayout_;
	ID3D11Buffer* pConstBuffer_;
	ID3D11VertexShader* pVertexShader_;

	PixelShader pixelShaderDefault_;
	PixelShader pixelShaderTexture_;

	SpriteManager();				// コンストラクタ
	~SpriteManager(){ UnInit(); }	// デストラクタ

	// コピーコンストラクタの禁止
	SpriteManager(const SpriteManager& src){}
	SpriteManager& operator=(const SpriteManager& src){}
};

#endif	// #ifndef INCLUDE_IDEA_SPRITEMANAGER_H