#include "../../h/2D/SpriteManager.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Utility/ideaUtility.h"
#include <stdio.h>

namespace{
	static const char* ARCHIVE_FILENAME = "shader.dat";
	static const char* VS_FILENAME = "VS2D.cso";
	static const char* PS_DEF_FILENAME = "PSDefault.cso";
	static const char* PS_TEX_FILENAME = "PSTexture.cso";
}

using namespace DirectX;

SpriteManager::SpriteManager() :
	pVertexShader_(nullptr),
	pVertexLayout_(nullptr),
	pRectVertexBuffer_(nullptr),
	pCircleVertexBuffer_(nullptr),
	pCircleIndexBuffer_(nullptr),
	pConstBuffer_(nullptr)
{}

bool SpriteManager::Init()
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return false; }

	HRESULT hr;

	{
		// 頂点シェーダの読み込み
		ArchiveLoader loader;
		if(!loader.Load(ARCHIVE_FILENAME, VS_FILENAME)){ return false; }

		// 頂点シェーダ作成
		hr = gm.GetDevicePtr()->CreateVertexShader(&loader.GetData()[0], loader.GetSize(), NULL, &pVertexShader_);
		if(FAILED(hr)){ return false; }

		// 入力レイアウト定義
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// 入力レイアウト作成
		hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, &loader.GetData()[0], loader.GetSize(), &pVertexLayout_);
		if(FAILED(hr)){ return false; }
	}

	// ピクセルシェーダの読み込み(通常)
	if(!pixelShaderDefault_.LoadPixelShaderFromCSO(PS_DEF_FILENAME)){ return false; }
	// ピクセルシェーダの読み込み(テクスチャ)
	if(!pixelShaderTexture_.LoadPixelShaderFromCSO(PS_TEX_FILENAME)){ return false; }


	// 定数バッファ
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(ConstBuffer2D);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pConstBuffer_);
		if(FAILED(hr)){ return false; }

		// 定数バッファ用意
		ConstBuffer2D cbuff;
		float w = (float)gm.GetWidth(), h = (float)gm.GetHeight();
		XMMATRIX mtx(
			2.0f / w, 0.0f, 0.0f, 0.0f,
			0.0f, -2.0f / h, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f, 1.0f
		);
		XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(mtx));
		
		// 定数バッファ内容更新
		gm.GetContextPtr()->UpdateSubresource(pConstBuffer_, 0, NULL, &cbuff, 0, 0);
		
		// 定数バッファのセット
		UINT cb_slot = 0;
		ID3D11Buffer* cb[1] ={ pConstBuffer_ };

		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}

	// 頂点バッファ
	// 矩形
	{
		VertexData2D v[RECT_VERTEX_NUM] ={};
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VertexData2D) * RECT_VERTEX_NUM;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = v;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pRectVertexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	// 円
	{
		VertexData2D v[CIRCLE_VERTEX_NUM] ={};
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.ByteWidth = sizeof(VertexData2D) * CIRCLE_VERTEX_NUM;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = v;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pCircleVertexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	// インデックスバッファ
	// 円
	{
		WORD indexList[(CIRCLE_VERTEX_NUM - 1) * 3 + 1] ={};
		int i = 0;
		int j = 1;
		for(; j < CIRCLE_VERTEX_NUM - 1;){
			indexList[i++] = 0;
			indexList[i++] = j;
			indexList[i++] = ++j;
		}
		indexList[i] = 0;

		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(WORD) * ((CIRCLE_VERTEX_NUM - 1) * 3 + 1);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = indexList;
		InitData.SysMemPitch      = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pCircleIndexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	return true;
}

void SpriteManager::UnInit()
{
	SafeRelease(pCircleIndexBuffer_);	
	SafeRelease(pCircleVertexBuffer_);
	SafeRelease(pRectVertexBuffer_);
	SafeRelease(pConstBuffer_);
	SafeRelease(pVertexLayout_);
	SafeRelease(pVertexShader_);
}

