/*==============================================================================
[Cube.cpp]
Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../h/3D/Cube.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/Environment/Camera.h"
#include "../../h/Environment/ShadowCamera.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../../idea/h/Texture/Texture.h"

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace DirectX;

Cube::Cube() :
	uNum_(1),
	vNum_(1),
	bDivided_(false),
	bDelimited_(false),
	pScmr_(nullptr)
{
}

void Cube::SetTexture(Texture& tex)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = false;
	bDelimited_ = false;
	uNum_ = 1;
	vNum_ = 1;
}

void Cube::SetDividedTexture(Texture& tex, int uNum, int vNum)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = true;
	bDelimited_ = false;
	uNum_ = uNum;
	vNum_ = vNum;
}

void Cube::SetDelimitedTexture(Texture & tex, float u, float v, float width, float height)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = false;
	bDelimited_ = true;
	uv_.x = u;
	uv_.y = v;
	size_.x = width;
	size_.y = height;
}

void Cube::ExclusionTexture()
{
	TextureHolder::ExclusionTexture();
	bDivided_ = false;
	bDelimited_ = false;
	uNum_ = 1;
	vNum_ = 1;
}

void Cube::SetShadow(ShadowCamera& scmr)
{
	pScmr_ = &scmr;
}

void Cube::Draw(Camera* pCamera)
{
	if(pScmr_ && !pScmr_->GetShadowDrawFlag()){
		if(!pTex_){
			DrawShadowCube(pCamera);
		} else if(bDivided_){
			DrawDividedTextureShadowCube(pCamera, *pTex_, uNum_, vNum_);
		} else if(bDelimited_){
			DrawDelimitedTextureShadowCube(pCamera, *pTex_, uv_.x, uv_.y, size_.x, size_.y);
		} else{
			DrawTextureShadowCube(pCamera, *pTex_);
		}
	} else{
		if(!pTex_){
			DrawCube(pCamera);
		} else if(bDivided_){
			DrawDividedTextureCube(pCamera, *pTex_, uNum_, vNum_);
		} else if(bDelimited_){
			DrawDelimitedTextureCube(pCamera, *pTex_, uv_.x, uv_.y, size_.x, size_.y);
		} else{
			DrawTextureCube(pCamera, *pTex_);
		}
	}
}

void Cube::DrawCube(Camera* pCamera, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetCubeVertexBufferPtr()
		|| !om.GetCubeIndexBufferPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){
		return;
	}

	//�萔�o�b�t�@
	ConstBuffer3D cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	// �萔�o�b�t�@���e�X�V
	gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// �萔�o�b�t�@
	UINT cb_slot = 1;
	ID3D11Buffer* cb[1] = { om.GetConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(ObjectManager::CUBE_INDEX_NUM, 0, 0);
}

inline void Cube::DrawShadowCube(Camera* pCamera, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetCubeVertexBufferPtr()
		|| !om.GetCubeIndexBufferPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){
		return;
	}

	//�萔�o�b�t�@
	ConstBuffer3DShadow cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	XMFLOAT4X4 matLightView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightView.m[i][j] = pScmr_->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightView, XMMatrixTranspose(XMLoadFloat4x4(&matLightView)));

	XMFLOAT4X4 matLightProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightProj.m[i][j] = pScmr_->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightProj, XMMatrixTranspose(XMLoadFloat4x4(&matLightProj)));

	// �萔�o�b�t�@���e�X�V
	gm.GetContextPtr()->UpdateSubresource(om.GetShadowConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// �萔�o�b�t�@
	UINT cb_slot = 3;
	ID3D11Buffer* cb[1] = { om.GetShadowConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());
	

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtr(1);

	gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(ObjectManager::CUBE_INDEX_NUM, 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

void Cube::DrawTextureCube(Camera* pCamera, const Texture & tex, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetCubeVertexBufferPtr()
		|| !om.GetCubeIndexBufferPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){
		return;
	}

	//�萔�o�b�t�@
	ConstBuffer3D cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	// �萔�o�b�t�@���e�X�V
	gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// �萔�o�b�t�@
	UINT cb_slot = 1;
	ID3D11Buffer* cb[1] = { om.GetConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);

	VertexData3D vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(VertexData3D) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(ObjectManager::CUBE_INDEX_NUM, 0, 0);
}

inline void Cube::DrawTextureShadowCube(Camera* pCamera, const Texture& tex, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetCubeVertexBufferPtr()
		|| !om.GetCubeIndexBufferPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){
		return;
	}

	//�萔�o�b�t�@
	ConstBuffer3DShadow cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	XMFLOAT4X4 matLightView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightView.m[i][j] = pScmr_->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightView, XMMatrixTranspose(XMLoadFloat4x4(&matLightView)));

	XMFLOAT4X4 matLightProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightProj.m[i][j] = pScmr_->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightProj, XMMatrixTranspose(XMLoadFloat4x4(&matLightProj)));

	// �萔�o�b�t�@���e�X�V
	gm.GetContextPtr()->UpdateSubresource(om.GetShadowConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// �萔�o�b�t�@
	UINT cb_slot = 3;
	ID3D11Buffer* cb[1] = { om.GetShadowConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);

	VertexData3D vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(VertexData3D) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// �e�N�X�`����������
	{
		ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
		}
	}
	{
		ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtr(1);

		gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
	}
	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(tex.GetTextureViewPtr()){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderTextureShadowPtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(ObjectManager::CUBE_INDEX_NUM, 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

void Cube::DrawDividedTextureCube(Camera* pCamera, const Texture & tex, int uNum, int vNum, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetCubeVertexBufferPtr()
		|| !om.GetCubeIndexBufferPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){
		return;
	}

	//�萔�o�b�t�@
	ConstBuffer3D cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	// �萔�o�b�t�@���e�X�V
	gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// �萔�o�b�t�@
	UINT cb_slot = 1;
	ID3D11Buffer* cb[1] = { om.GetConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);

	// ���_�o�b�t�@
	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);

	VertexData3D vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(VertexData3D) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(ObjectManager::CUBE_INDEX_NUM, 0, 0);
}

inline void Cube::DrawDividedTextureShadowCube(Camera* pCamera, const Texture& tex, int uNum, int vNum, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetCubeVertexBufferPtr()
		|| !om.GetCubeIndexBufferPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){
		return;
	}

	//�萔�o�b�t�@
	ConstBuffer3DShadow cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	XMFLOAT4X4 matLightView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightView.m[i][j] = pScmr_->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightView, XMMatrixTranspose(XMLoadFloat4x4(&matLightView)));

	XMFLOAT4X4 matLightProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightProj.m[i][j] = pScmr_->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightProj, XMMatrixTranspose(XMLoadFloat4x4(&matLightProj)));

	// �萔�o�b�t�@���e�X�V
	gm.GetContextPtr()->UpdateSubresource(om.GetShadowConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// �萔�o�b�t�@
	UINT cb_slot = 3;
	ID3D11Buffer* cb[1] = { om.GetShadowConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);

	// ���_�o�b�t�@
	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);

	VertexData3D vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(VertexData3D) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// �e�N�X�`����������
	{
		ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
		}
	}
	{
		ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtr(1);

		gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
	}
	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(tex.GetTextureViewPtr()){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderTextureShadowPtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(ObjectManager::CUBE_INDEX_NUM, 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

inline void Cube::DrawDelimitedTextureCube(Camera * pCamera, const Texture & tex, float u, float v, float width, float height, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetCubeVertexBufferPtr()
		|| !om.GetCubeIndexBufferPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){
		return;
	}

	//�萔�o�b�t�@
	ConstBuffer3D cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	// �萔�o�b�t�@���e�X�V
	gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// �萔�o�b�t�@
	UINT cb_slot = 1;
	ID3D11Buffer* cb[1] = { om.GetConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);

	// ���_�o�b�t�@
	float u1 = u;
	float u2 = u + width;
	float v1 = v;
	float v2 = v + height;

	VertexData3D vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(VertexData3D) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(ObjectManager::CUBE_INDEX_NUM, 0, 0);
}

inline void Cube::DrawDelimitedTextureShadowCube(Camera* pCamera, const Texture& tex, float u, float v, float width, float height, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetCubeVertexBufferPtr()
		|| !om.GetCubeIndexBufferPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){
		return;
	}

	//�萔�o�b�t�@
	ConstBuffer3DShadow cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	XMFLOAT4X4 matLightView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightView.m[i][j] = pScmr_->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightView, XMMatrixTranspose(XMLoadFloat4x4(&matLightView)));

	XMFLOAT4X4 matLightProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightProj.m[i][j] = pScmr_->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightProj, XMMatrixTranspose(XMLoadFloat4x4(&matLightProj)));

	// �萔�o�b�t�@���e�X�V
	gm.GetContextPtr()->UpdateSubresource(om.GetShadowConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// �萔�o�b�t�@
	UINT cb_slot = 3;
	ID3D11Buffer* cb[1] = { om.GetShadowConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);

	// ���_�o�b�t�@
	float u1 = u;
	float u2 = u + width;
	float v1 = v;
	float v2 = v + height;

	VertexData3D vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f,  0.0f, -1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f,  0.0f,  1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f,  0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ { -0.5f, -0.5f, -0.5f }, { -1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ {  0.5f,  0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ {  0.5f, -0.5f,  0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  0.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },

	{ { -0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f,  0.5f,  0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ {  0.5f,  0.5f, -0.5f }, {  0.0f,  1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },

	{ { -0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } },
	{ {  0.5f, -0.5f,  0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f, -0.5f, -0.5f }, {  0.0f, -1.0f,  0.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetCubeVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(VertexData3D) * ObjectManager::CUBE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �C���f�b�N�X�o�b�t�@�̃Z�b�g
	gm.GetContextPtr()->IASetIndexBuffer(om.GetCubeIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// �e�N�X�`����������
	{
		ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
		}
	}
	{
		ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtr(1);

		gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
	}
	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(tex.GetTextureViewPtr()){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderTextureShadowPtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed(ObjectManager::CUBE_INDEX_NUM, 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}
