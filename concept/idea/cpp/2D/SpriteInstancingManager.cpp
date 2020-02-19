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

////�V�F�[�_�萔�o�b�t�@
//struct ConstBuffer2D
//{
//	XMFLOAT4X4 proj;
//};

SpriteInstancingManager::SpriteInstancingManager() :
	pVertexShader_(nullptr),
	//pVertexLayout_(nullptr),
	//pRectVertexBuffer_(nullptr),
	//pConstBuffer_(nullptr),
	pInstanceDataBuffer_(nullptr),
	pTransformShaderResourceView_(nullptr)
{}


bool SpriteInstancingManager::Init()
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return false; }

	HRESULT hr;

	{
		// ���_�V�F�[�_�쐬
		ArchiveLoader loader;
		if(!loader.Load(ARCHIVE_FILENAME, VS_FILENAME)){ return false; }
		hr = gm.GetDevicePtr()->CreateVertexShader(&loader.GetData()[0], loader.GetSize(), NULL, &pVertexShader_);
		if(FAILED(hr)){ return false; }

		// ���̓��C�A�E�g��`
		//D3D11_INPUT_ELEMENT_DESC layout[] = {
		//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//};
		//UINT elem_num = ARRAYSIZE(layout);

		//// ���̓��C�A�E�g�쐬
		//hr = gm.GetDevicePtr()->CreateInputLayout(layout, elem_num, &loader.GetData()[0], loader.GetSize(), &pVertexLayout_);
		//if(FAILED(hr)){ return false; }
	}

	// �萔�o�b�t�@
	//{
	//	D3D11_BUFFER_DESC bd;
	//	ZeroMemory(&bd, sizeof(bd));
	//	bd.ByteWidth = sizeof(ConstBuffer2D);
	//	bd.Usage = D3D11_USAGE_DEFAULT;
	//	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//	bd.CPUAccessFlags = 0;
	//	bd.MiscFlags = 0;
	//	bd.StructureByteStride = 0;
	//	hr = gm.GetDevicePtr()->CreateBuffer(&bd, nullptr, &pConstBuffer_);
	//	if(FAILED(hr)){ return false; }

	//	// �萔�o�b�t�@�p��
	//	ConstBuffer2D cbuff;
	//	float w = (float)gm.GetWidth(), h = (float)gm.GetHeight();
	//	XMMATRIX mtx(
	//		2.0f / w, 0.0f, 0.0f, 0.0f,
	//		0.0f, -2.0f / h, 0.0f, 0.0f,
	//		0.0f, 0.0f, 1.0f, 0.0f,
	//		-1.0f, 1.0f, 0.0f, 1.0f
	//	);
	//	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(mtx));

	//	// �萔�o�b�t�@���e�X�V
	//	gm.GetContextPtr()->UpdateSubresource(pConstBuffer_, 0, NULL, &cbuff, 0, 0);

	//	// �萔�o�b�t�@�̃Z�b�g
	//	UINT cb_slot = 0;
	//	ID3D11Buffer* cb[1] ={ pConstBuffer_ };
	//	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	//}

	// �X�g���N�`���o�b�t�@
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

		// Create ShaderResourceView
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory( &srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC) );
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;   // �g�����ꂽ�o�b�t�@�[�ł��邱�Ƃ��w�肷��
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.BufferEx.NumElements = INSTANCE_MAX; // ���\�[�X���̗v�f�̐�

		// �\�����o�b�t�@�[�����ƂɃV�F�[�_�[���\�[�X�r���[���쐬����
		hr = gm.GetDevicePtr()->CreateShaderResourceView( pInstanceDataBuffer_, &srvDesc, &pTransformShaderResourceView_);
		if(FAILED(hr)){ return false; }
	}

	// ���_�o�b�t�@
	//{
	//	VertexData2D v[SpriteManager::RECT_VERTEX_NUM] ={};
	//	D3D11_BUFFER_DESC bd;
	//	ZeroMemory(&bd, sizeof(bd));
	//	bd.Usage = D3D11_USAGE_DYNAMIC;
	//	bd.ByteWidth = sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM;
	//	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	bd.MiscFlags = 0;
	//	D3D11_SUBRESOURCE_DATA InitData;
	//	ZeroMemory(&InitData, sizeof(InitData));
	//	InitData.pSysMem = v;
	//	hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pRectVertexBuffer_);
	//	if(FAILED(hr)){ return false; }
	//}

	return true;
}

void SpriteInstancingManager::UnInit()
{
	//SafeRelease(pRectVertexBuffer_);
	SafeRelease(pTransformShaderResourceView_);
	SafeRelease(pInstanceDataBuffer_);
	//SafeRelease(pConstBuffer_);
	//SafeRelease(pVertexLayout_);
	SafeRelease(pVertexShader_);
}
