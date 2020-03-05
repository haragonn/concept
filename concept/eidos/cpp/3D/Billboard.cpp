/*==============================================================================
[Billboard.cpp]
Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../h/3D/Billboard.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/Environment/Camera.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../../idea/h/Texture/Texture.h"

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace DirectX;

void Billboard::SetTexture(Texture & tex)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = false;
	uNum_ = 0;
	vNum_ = 0;
}

void Billboard::SetDividedTexture(Texture & tex, int uNum, int vNum)
{
	TextureHolder::SetTexture(&tex);
	bDivided_ = true;
	uNum_ = uNum;
	vNum_ = vNum;
}

void Billboard::ExclusionTexture()
{
	TextureHolder::ExclusionTexture();
	bDivided_ = false;
	uNum_ = 0;
	vNum_ = 0;
}

void Billboard::Draw(Camera * pCamera)
{
	UpdateMatrix(pCamera);

	if(!pTex_){
		DrawBillboard(pCamera);
	}else if(!bDivided_){
		DrawTextureBillboard(pCamera, *pTex_);
	}else{
		DrawDividedTextureBillboard(pCamera, *pTex_, uNum_, vNum_);
	}
}

inline void Billboard::DrawBillboard(Camera * pCamera, int blend)
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

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

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
	gm.GetContextPtr()->Draw(4, 0);
}

inline void Billboard::DrawTextureBillboard(Camera * pCamera, const Texture & tex, int blend)
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
	{ { -0.5f,  0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 0.0f } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 0.0f } },
	{ { -0.5f, -0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 0.0f, 1.0f } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { 1.0f, 1.0f } }
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetBillboardVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(VertexData3D) * 4);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

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
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	}else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}

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
	gm.GetContextPtr()->Draw(4, 0);
}

inline void Billboard::DrawDividedTextureBillboard(Camera * pCamera, const Texture & tex, int uNum, int vNum, int blend)
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

	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);

	VertexData3D vertexList[]{
	{ { -0.5f,  0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v1 } },
	{ {  0.5f,  0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v1 } },
	{ { -0.5f, -0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u1, v2 } },
	{ {  0.5f, -0.5f, -0.5f }, {  1.0f,  1.0f, 1.0f }, {1.0f, 1.0f, 1.0f, color_.a}, { u2, v2 } }
	};

	// �o�b�t�@��������
	ID3D11Buffer* pVBuf = om.GetBillboardVertexBufferPtr();
	D3D11_MAPPED_SUBRESOURCE msr;
	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vertexList, sizeof(VertexData3D) * 4);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData3D);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

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
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	}else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}

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
	gm.GetContextPtr()->Draw(4, 0);
}

inline void Billboard::UpdateMatrix(Camera* pCamera)
{
	XMMATRIX matWorld, matScale, matRot, matViewInv;

	matWorld = XMMatrixIdentity();

	XMMATRIX matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.r[i].m128_f32[j] = pCamera->GetViewMatrix().r[i][j];
		}
	}

	matViewInv = XMMatrixTranspose(matView);
	matViewInv.r[0].m128_f32[3] = 0.0f;
	matViewInv.r[1].m128_f32[3] = 0.0f;
	matViewInv.r[2].m128_f32[3] = 0.0f;

	if(bYBillboard_){
		matViewInv.r[0].m128_f32[1] = 0.0f;
		matViewInv.r[1].m128_f32[0] = 0.0f;
		matViewInv.r[1].m128_f32[1] = 1.0f;
		matViewInv.r[1].m128_f32[2] = 0.0f;
		matViewInv.r[2].m128_f32[1] = 0.0f;
	}

	// �g��k������
	matScale = XMMatrixScaling(scale_.x, scale_.y, 0.0f);

	// 0.0f �` ideaMath::PI * 2.0f�͈̔͂ɐ��K��
	while(rot_.x < 0.0f)
	{
		rot_.x += ideaMath::PI * 2.0f;
	}
	while(rot_.x > ideaMath::PI * 2.0f)
	{
		rot_.x -= ideaMath::PI * 2.0f;
	}

	while(rot_.y < 0.0f)
	{
		rot_.y += ideaMath::PI * 2.0f;
	}
	while(rot_.y > ideaMath::PI * 2.0f)
	{
		rot_.y -= ideaMath::PI * 2.0f;
	}

	while(rot_.z < 0.0f)
	{
		rot_.z += ideaMath::PI * 2.0f;
	}
	while(rot_.z > ideaMath::PI * 2.0f)
	{
		rot_.z -= ideaMath::PI * 2.0f;
	}

	// ��]����
	matRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, rot_.z);

	// �s��̍���
	matWorld = XMMatrixMultiply(matWorld, matScale);
	matWorld = XMMatrixMultiply(matWorld, matRot);
	matWorld = matWorld * matViewInv;

	// ���s�ړ�
	matWorld.r[3].m128_f32[0] = pos_.x;
	matWorld.r[3].m128_f32[1] = pos_.y;
	matWorld.r[3].m128_f32[2] = pos_.z;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			world_.r[i][j] = matWorld.r[i].m128_f32[j];
		}
	}
}