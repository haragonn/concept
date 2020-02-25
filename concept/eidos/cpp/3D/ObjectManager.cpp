#include "../../h/3D/ObjectManager.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../../idea//h/Shader/Shader.h"
#include "../../../idea/h/Archive/ArchiveLoader.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../idea/h/Utility/ideaUtility.h"

namespace{
	static const char* ARCHIVE_FILENAME = "shader.dat";
	static const char* VS_FILENAME = "VS3D.cso";
}
using namespace DirectX;

ObjectManager::ObjectManager() :
	pVertexShader_(nullptr),
	pPmdVertexShader_(nullptr),
	pVertexLayout_(nullptr),
	pPmdVertexLayout_(nullptr),
	pCubeVertexBuffer_(nullptr),
	pCubeIndexBuffer_(nullptr),
	pBillboardVertexBuffer_(nullptr),
	pBillboardIndexBuffer_(nullptr),
	pConstBuffer_(nullptr),
	pPmdConstBuffer_(nullptr)
{
	light_ = XMVector3Normalize(XMVectorSet(0.2f, 1.0f, -0.5f, 0.0f));
}

bool ObjectManager::Init()
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return false; }

	HRESULT hr;

	{
		// 頂点シェーダの読み込み
		{
			// 頂点シェーダ作成
			ArchiveLoader loader;
			if(!loader.Load(ARCHIVE_FILENAME, VS_FILENAME)){ return false; }
			hr = gm.GetDevicePtr()->CreateVertexShader(&loader.GetData()[0], loader.GetSize(), NULL, &pVertexShader_);
			if(FAILED(hr)){ return false; }

			//BYTE* data;
			//int size = 0;
			//size = ReadShader("VS3D.cso", &data);
			//if(size == 0){ return false; }
			//hr = gm.GetDevicePtr()->CreateVertexShader(data, size, NULL, &pVertexShader_);
			//if(FAILED(hr)){ return false; }

			// 入力レイアウト定義
			D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			UINT elem_num = ARRAYSIZE(layout);

			// 入力レイアウト作成
			hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, &loader.GetData()[0], loader.GetSize(), &pVertexLayout_);
			//hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, data, size, &pVertexLayout_);
			//delete[] data;
			if(FAILED(hr)){ return false; }
		}
		// 頂点シェーダの読み込み
		{
			// 頂点シェーダ作成
			//ArchiveLoader loader;
			//if(!loader.Load(ARCHIVE_FILENAME, VS_FILENAME)){ return false; }
			//hr = gm.GetDevicePtr()->CreateVertexShader(&loader.GetData()[0], loader.GetSize(), NULL, &pVertexShader_);
			//if(FAILED(hr)){ return false; }

			BYTE* data;
			int size = 0;
			size = ReadShader("VS3DPMD.cso", &data);
			if(size == 0){ return false; }
			hr = gm.GetDevicePtr()->CreateVertexShader(data, size, NULL, &pPmdVertexShader_);
			if(FAILED(hr)){ return false; }

			// 入力レイアウト定義
			D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			};
			UINT elem_num = ARRAYSIZE(layout);

			// 入力レイアウト作成
			//hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, &loader.GetData()[0], loader.GetSize(), &pVertexLayout_);
			hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, data, size, &pPmdVertexLayout_);
			delete[] data;
			if(FAILED(hr)){ return false; }
		}
	}

	// ピクセルシェーダの読み込み(通常)
	if(!pixelShaderDefault_.LoadPixelShaderFromCSO("PSDefault.cso")){ return false; }
	// ピクセルシェーダの読み込み(テクスチャ)
	if(!pixelShaderTexture_.LoadPixelShaderFromCSO("PSTexture.cso")){ return false; }

	// 定数バッファ
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(ConstBuffer3D);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pConstBuffer_);
		if(FAILED(hr)){ return false; }

		XMMATRIX worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		XMVECTOR eye = XMVectorSet(2.0f, 2.0f, -2.0f, 0.0f);
		XMVECTOR focus = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		XMMATRIX viewMatrix = XMMatrixLookAtLH(eye, focus, up);

		float fov = XMConvertToRadians(60);
		float aspect = (float)gm.GetWidth() / gm.GetHeight();
		float nearZ = 0.1f;
		float farZ = 100.0f;
		XMMATRIX projMatrix = XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);

		XMVECTOR light = XMVector3Normalize(XMVectorSet(0.0f, 0.5f, -1.0f, 0.0f));

		//定数バッファ
		ConstBuffer3D cbuff;

		XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(worldMatrix));
		XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(viewMatrix));
		XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(projMatrix));
		XMStoreFloat4(&cbuff.color, XMVector3Normalize(XMVectorSet(1.0f, 1.0f, 1.0f,1.0f)));
		XMStoreFloat4(&cbuff.light, light);

		// 定数バッファ内容更新
		gm.GetContextPtr()->UpdateSubresource(pConstBuffer_, 0, NULL, &cbuff, 0, 0);

		// 定数バッファセット
		UINT cb_slot = 1;
		ID3D11Buffer* cb[1] = { pConstBuffer_ };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(ConstBufferBoneWorld);
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pPmdConstBuffer_);
		if(FAILED(hr)){ return false; }

		//定数バッファ
		ConstBufferBoneWorld cbuff;
		ZeroMemory(&cbuff, sizeof(cbuff));

		// ボーンワールド行列の初期化
		XMMATRIX matWorld = XMMatrixIdentity();

		for(int i = 0; i < 512; ++i){
			XMStoreFloat4x4(&cbuff.boneWorld[i], matWorld);
		}

		// 定数バッファ内容更新
		gm.GetContextPtr()->UpdateSubresource(pPmdConstBuffer_, 0, NULL, &cbuff, 0, 0);

		// 定数バッファセット
		UINT cb_slot = 2;
		ID3D11Buffer* cb[1] = { pPmdConstBuffer_ };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}

	// 頂点バッファ
	{
		VertexData3D cubeVertexList[]{
		{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f } },

		{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f } },

		{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },
		{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },

		{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f } },

		{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f } },

		{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f } },
		{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f } },
		};


		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(VertexData3D) * CUBE_VERTEX_NUM;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = cubeVertexList;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pCubeVertexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	// インデックスバッファ
	{
		WORD cubeIndexList[]{
			0,  1,  2,     3,  2,  1,
			4,  5,  6,     7,  6,  5,
			8,  9, 10,    11, 10,  9,
			12, 13, 14,    15, 14, 13,
			16, 17, 18,    19, 18, 17,
			20, 21, 22,    23, 22, 21,
		};

		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(WORD) * CUBE_INDEX_NUM;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = cubeIndexList;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pCubeIndexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	// 頂点バッファ
	{
		VertexData3D billboardVertexList[]{
		{ { -0.5f,  0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, { 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, { 1.0f, 1.0f } }
		};

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(VertexData3D) * 4;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = billboardVertexList;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pBillboardVertexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	return true;
}

void ObjectManager::UnInit()
{
	SafeRelease(pBillboardIndexBuffer_);
	SafeRelease(pBillboardVertexBuffer_);
	SafeRelease(pCubeIndexBuffer_);
	SafeRelease(pCubeVertexBuffer_);
	SafeRelease(pConstBuffer_);
	SafeRelease(pPmdConstBuffer_);
	SafeRelease(pVertexLayout_);
	SafeRelease(pPmdVertexLayout_);
	SafeRelease(pVertexShader_);
	SafeRelease(pPmdVertexShader_);
}

void ObjectManager::SetLight(float axisX, float axisY, float axisZ)
{
	light_ = XMVector3Normalize(XMVectorSet(axisX, axisY, axisZ, 0.0f));
}
