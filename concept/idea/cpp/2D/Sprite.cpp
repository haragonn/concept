/*==============================================================================
	[Sprite.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/2D/Sprite.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/2D/SpriteInstancing.h"
#include "../../h/Texture/Texture.h"
#include "../../h/Utility/ideaMath.h"

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace std;
using namespace DirectX;

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
Sprite::Sprite()
{
	vector<SpriteInstancing*>().swap(vecSpriteInstancingPtr_);
}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
Sprite::~Sprite()
{
	if(vecSpriteInstancingPtr_.size() > 0){
		for(auto it = begin(vecSpriteInstancingPtr_), itEnd = end(vecSpriteInstancingPtr_); it != itEnd; ++it){
			if(*it){
				(*it)->RemoveSprite(*this);
			}
		}
	}
}

//------------------------------------------------------------------------------
// ��`�̕`��
// �����@�F�������@(int blend)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Sprite::DrawRect(int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sm.GetVertexShederPtr()
		|| !sm.GetPixelShederDefaultPtr()){
		return;
	}

	// ��ʊO�Ȃ�I��
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight);

	if(pos_.x + longVicinity * ROOT2 < 0.0f
		|| pos_.x - longVicinity * ROOT2 > gm.GetWidth()
		|| pos_.y + longVicinity * ROOT2 < 0.0f
		|| pos_.y - longVicinity * ROOT2 > gm.GetHeight()){
		return;
	}

	// ���_���
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];

	vd[0] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 0.0f) };
	vd[1] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 0.0f) };
	vd[2] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 1.0f) };
	vd[3] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 1.0f) };

	// ���_���̌v�Z
	float x, y, axisX, axisY;
	float tSin = sinf(rad_);
	float tCos = cosf(rad_);

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX + pos_.x;
		vd[i].pos.y = axisY + pos_.y;
	}

	// �o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->Draw(SpriteManager::RECT_VERTEX_NUM, 0);
}

//------------------------------------------------------------------------------
// �~�̕`��
// �����@�F�`��͈�(float ratio),�������@(int blend)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Sprite::DrawCircle(float ratio, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetCircleVertexBufferPtr()
		|| !sm.GetVertexShederPtr()
		|| !sm.GetPixelShederDefaultPtr()){
		return;
	}

	// ��ʊO�Ȃ�I��
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight);

	if(pos_.x + longVicinity * ROOT2 < 0.0f
		|| pos_.x - longVicinity * ROOT2 > gm.GetWidth()
		|| pos_.y + longVicinity * ROOT2 < 0.0f
		|| pos_.y - longVicinity * ROOT2 > gm.GetHeight()){
		return;
	}

	// 1.0f�ȏ�Ȃ�]������
	if(ratio > 1.0f){ ratio = ratio - (int)ratio; }
	// 0.0f�ȉ��Ȃ�I��
	if(ratio <= 0.0f){ return; }

	// ���_���
	VertexData2D vd[SpriteManager::CIRCLE_VERTEX_NUM] = {};
	vd[0].pos.x = pos_.x;
	vd[0].pos.y = pos_.y;
	vd[0].color.x = color_.r;
	vd[0].color.y = color_.g;
	vd[0].color.z = color_.b;
	vd[0].color.w = color_.a;

	// ���_���̌v�Z
	for(int i = 1; i < SpriteManager::CIRCLE_VERTEX_NUM; ++i){
		vd[i].pos.x = pos_.x + max(halfWidth, halfHeight) * -sinf(ideaPI * 2.0f - rad_ - ideaPI * 2.0f / (SpriteManager::CIRCLE_VERTEX_NUM - 2) * (i - 1));
		vd[i].pos.y = pos_.y + max(halfWidth, halfHeight) * -cosf(ideaPI * 2.0f - rad_ - ideaPI * 2.0f / (SpriteManager::CIRCLE_VERTEX_NUM - 2) * (i - 1));
		vd[i].color.x = color_.r;
		vd[i].color.y = color_.g;
		vd[i].color.z = color_.b;
		vd[i].color.w = color_.a;
	}

	// ���_�o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetCircleVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::CIRCLE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);
	gm.GetContextPtr()->IASetIndexBuffer(sm.GetCircleIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed((int)((SpriteManager::CIRCLE_VERTEX_NUM - 2) * ratio) * 3, 0, 0);
}

//------------------------------------------------------------------------------
// �����̕`��
// �����@�F�`��͈�(float ratio),�������@(int blend)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Sprite::DrawPhoton(float ratio, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetCircleVertexBufferPtr()
		|| !sm.GetVertexShederPtr()
		|| !sm.GetPixelShederDefaultPtr()){
		return;
	}

	// ��ʊO�Ȃ�I��
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight);

	if(pos_.x + longVicinity * ROOT2 < 0.0f
		|| pos_.x - longVicinity * ROOT2 > gm.GetWidth()
		|| pos_.y + longVicinity * ROOT2 < 0.0f
		|| pos_.y - longVicinity * ROOT2 > gm.GetHeight()){
		return;
	}

	// 1.0f�ȏ�Ȃ�]������
	if(ratio > 1.0f){ ratio = ratio - (int)ratio; }
	// 0.0f�ȉ��Ȃ�I��
	if(ratio <= 0.0f){ return; }

	// ���_���
	VertexData2D vd[SpriteManager::CIRCLE_VERTEX_NUM] = {};
	vd[0].pos.x = pos_.x;
	vd[0].pos.y = pos_.y;
	vd[0].color.x = color_.r;
	vd[0].color.y = color_.g;
	vd[0].color.z = color_.b;
	vd[0].color.w = color_.a;

	// ���_���̌v�Z
	for(int i = 1; i < SpriteManager::CIRCLE_VERTEX_NUM; ++i){
		vd[i].pos.x = pos_.x + max(halfWidth, halfHeight) * -sinf(ideaPI * 2.0f - rad_ - ideaPI * 2.0f / (SpriteManager::CIRCLE_VERTEX_NUM - 2) * (i - 1));
		vd[i].pos.y = pos_.y + max(halfWidth, halfHeight) * -cosf(ideaPI * 2.0f - rad_ - ideaPI * 2.0f / (SpriteManager::CIRCLE_VERTEX_NUM - 2) * (i - 1));
		vd[i].color.x = color_.r;
		vd[i].color.y = color_.g;
		vd[i].color.z = color_.b;
		vd[i].color.w = 0.0f;
	}

	// �o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetCircleVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::CIRCLE_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);
	gm.GetContextPtr()->IASetIndexBuffer(sm.GetCircleIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}
	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->DrawIndexed((int)((SpriteManager::CIRCLE_VERTEX_NUM - 2) * ratio) * 3, 0, 0);
}

//------------------------------------------------------------------------------
// �e�N�X�`���̕`��
// �����@�F�e�N�X�`��(const Texture& tex),�������@(int blend)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Sprite::DrawTexture(const Texture& tex, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sm.GetVertexShederPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetPixelShederTexturePtr()){
		return;
	}

	// ��ʊO�Ȃ�I��
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight);

	if(pos_.x + longVicinity * ROOT2 < 0.0f
		|| pos_.x - longVicinity * ROOT2 > gm.GetWidth()
		|| pos_.y + longVicinity * ROOT2 < 0.0f
		|| pos_.y - longVicinity * ROOT2 > gm.GetHeight()){
		return;
	}

	// ���_���
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 0.0f) };
	vd[1] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 0.0f) };
	vd[2] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 1.0f) };
	vd[3] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 1.0f) };

	// ���_���̌v�Z
	float x, y, axisX, axisY;
	float tSin = sinf(rad_);
	float tCos = cosf(rad_);

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX + pos_.x;
		vd[i].pos.y = axisY + pos_.y;
		vd[i].tex.x = (!!(i % 2) ^ bReversedU_) ? 1.0f : 0.0f;
		vd[i].tex.y = (!!(i > 1) ^ bReversedV_) ? 1.0f : 0.0f;
	}

	// �o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->Draw(SpriteManager::RECT_VERTEX_NUM, 0);
}

//------------------------------------------------------------------------------
// ���������ꂽ�e�N�X�`���̕`��
// �����@�F�e�N�X�`��(const Texture& tex),U���W(int uNum),
// �@�@�@�@V���W(int vNum),�������@(int blend)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Sprite::DrawDividedTexture(const Texture& tex, int uNum, int vNum, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sm.GetVertexShederPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetPixelShederTexturePtr()){
		return;
	}

	// ��ʊO�Ȃ�I��
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight);

	if(pos_.x + longVicinity * ROOT2 < 0.0f
		|| pos_.x - longVicinity * ROOT2 > gm.GetWidth()
		|| pos_.y + longVicinity * ROOT2 < 0.0f
		|| pos_.y - longVicinity * ROOT2 > gm.GetHeight()){
		return;
	}

	// ���_���
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 0.0f) };
	vd[1] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 0.0f) };
	vd[2] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 1.0f) };
	vd[3] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 1.0f) };

	// ���_���̌v�Z
	float x, y, axisX, axisY;
	float tSin = sinf(rad_);
	float tCos = cosf(rad_);
	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX + pos_.x;
		vd[i].pos.y = axisY + pos_.y;
		vd[i].tex.x = (!!(i % 2) ^ bReversedU_) ? u2 : u1;
		vd[i].tex.y = (!!(i > 1) ^ bReversedV_) ? v2 : v1;
	}

	// �o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->Draw(SpriteManager::RECT_VERTEX_NUM, 0);
}

//------------------------------------------------------------------------------
// �\���͈͂��w�肵���e�N�X�`���[�̕`��
// �����@�F�e�N�X�`��(const Texture& tex),U���W(int uNum),
// �@�@�@�@V���W(int vNum),��(float width),����(float width),
// �@�@�@�@�������@(int blend)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Sprite::DrawDelimitedTexture(const Texture& tex, float u, float v, float width, float height, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sm.GetVertexShederPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetPixelShederTexturePtr()){
		return;
	}

	// ��ʊO�Ȃ�I��
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;
	float longVicinity = max(halfWidth, halfHeight);

	if(pos_.x + longVicinity * ROOT2 < 0.0f
		|| pos_.x - longVicinity * ROOT2 > gm.GetWidth()
		|| pos_.y + longVicinity * ROOT2 < 0.0f
		|| pos_.y - longVicinity * ROOT2 > gm.GetHeight()){
		return;
	}

	// ���_���
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 0.0f) };
	vd[1] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 0.0f) };
	vd[2] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(0.0f, 1.0f) };
	vd[3] = { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(color_.r, color_.g, color_.b, color_.a), XMFLOAT2(1.0f, 1.0f) };

	// ���_���̌v�Z
	float x, y, axisX, axisY;
	float tSin = sinf(rad_);
	float tCos = cosf(rad_);
	float u1 = u;
	float u2 = u + width;
	float v1 = v;
	float v2 = v + height;

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX + pos_.x;
		vd[i].pos.y = axisY + pos_.y;
		vd[i].tex.x = (!!(i % 2) ^ bReversedU_) ? u2 : u1;
		vd[i].tex.y = (!!(i > 1) ^ bReversedV_) ? v2 : v1;
	}

	// �o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sm.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	//�|���S���`��
	gm.GetContextPtr()->Draw(SpriteManager::RECT_VERTEX_NUM, 0);
}