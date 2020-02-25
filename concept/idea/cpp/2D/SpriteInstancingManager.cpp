#include "../../h/2D/SpriteInstancingManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Utility/ideaUtility.h"
#include <stdio.h>

namespace{
	static const char* ARCHIVE_FILENAME = "shader.dat";
	static const char* VS_FILENAME = "VS2DI.cso";
}

using namespace DirectX;

SpriteInstancingManager::SpriteInstancingManager() :
	pVertexShader_(nullptr),
	pInstanceDataBuffer_(nullptr),
	pTransformShaderResourceView_(nullptr)
{}


bool SpriteInstancingManager::Init()
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return false; }

	HRESULT hr;

	{
		// 頂点シェーダ作成
		ArchiveLoader loader;
		if(!loader.Load(ARCHIVE_FILENAME, VS_FILENAME)){ return false; }

		hr = gm.GetDevicePtr()->CreateVertexShader(&loader.GetData()[0], loader.GetSize(), NULL, &pVertexShader_);
		if(FAILED(hr)){ return false; }
	}

	// ストラクチャバッファ
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(PerInstanceData) * INSTANCE_MAX;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED ;
		bd.StructureByteStride = sizeof(PerInstanceData);

		hr = gm.GetDevicePtr()->CreateBuffer( &bd, NULL, &pInstanceDataBuffer_ );
		if(FAILED(hr)){ return false; }

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory( &srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // 拡張されたバッファーであることを指定する
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.BufferEx.NumElements = INSTANCE_MAX; // リソース内の要素の数

		// 構造化バッファーをもとにシェーダーリソースビューを作成する
		hr = gm.GetDevicePtr()->CreateShaderResourceView( pInstanceDataBuffer_, &srvDesc, &pTransformShaderResourceView_);
		if(FAILED(hr)){ return false; }
	}

	return true;
}

void SpriteInstancingManager::UnInit()
{
	SafeRelease(pTransformShaderResourceView_);
	SafeRelease(pInstanceDataBuffer_);
	SafeRelease(pVertexShader_);
}
