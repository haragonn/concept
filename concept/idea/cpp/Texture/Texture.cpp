/*==============================================================================
	[Texture.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/Texture/Texture.h"
#include "../../h/Texture//TextureHolder.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Storage/StorageManager.h"
#include "../../h/Utility//ideaUtility.h"
#define WIN32_LEAN_AND_MEAN
#include "WICTextureLoader.h"
#include <d3d11.h>
#include <locale.h>

using namespace std;
using namespace DirectX;

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
Texture::Texture():
	pTexture_(nullptr),
	pTextureView_(nullptr),
	divU_(1.0f),
	divV_(1.0f),
	bStorage_(false)
{
	ZeroMemory(fileName_, ArraySize(fileName_));
	vector<TextureHolder*>().swap(vecTexHolderPtr_);
}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
Texture::~Texture()
{
	// �I������
	UnLoad();
}

//------------------------------------------------------------------------------
// �摜�̓ǂݍ���
// �����@�F�t�@�C����(const char* pFileName)
// �@�@�@�@���ɕ������鐔(unsigned int divX),�c�ɕ������鐔(unsigned int divY)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Texture::LoadImageFromFile(const char* pFileName, unsigned int divX, unsigned int divY)
{
	ID3D11Device* pD3DDev = GraphicManager::Instance().GetDevicePtr();
	if(!pD3DDev || pTexture_){ return false; }	// �f�o�C�X���Ȃ������ɓǂݍ��ݍς݂Ȃ�I��

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// ���������i�[����
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// �摜�̓ǂݍ���
	setlocale(LC_ALL, "japanese");
	wchar_t pTemp[MAX_PATH];
	size_t size;
	mbstowcs_s(&size, pTemp, MAX_PATH, pFileName, _TRUNCATE);

	HRESULT hr = CreateWICTextureFromFile(pD3DDev, pTemp, &pTexture_, &pTextureView_);

	if(FAILED(hr)){
		pTexture_ = nullptr;
		pTextureView_ = nullptr;
		SetDebugMessage("TexturLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
	}
	
	return SUCCEEDED(hr);
}

bool Texture::LoadImageFromArchiveFile(const char * pArchiveFileName, const char * pFileName, unsigned int divX, unsigned int divY)
{
	ID3D11Device* pD3DDev = GraphicManager::Instance().GetDevicePtr();
	if(!pD3DDev || pTexture_){ return false; }	// �f�o�C�X���Ȃ������ɓǂݍ��ݍς݂Ȃ�I��

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// ���������i�[����
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// �摜�̓ǂݍ���
	ArchiveLoader loader;
	if(!loader.Load(pArchiveFileName, pFileName)){
		SetDebugMessage("TexturLoadError! [%s] �� [%s] ����ǂݍ��߂܂���ł���\n", pFileName, pArchiveFileName);
		return false;
	}

	setlocale(LC_ALL, "japanese");
	wchar_t pTemp[MAX_PATH];
	size_t size;
	mbstowcs_s(&size, pTemp, MAX_PATH, pFileName, _TRUNCATE);
	HRESULT hr = CreateWICTextureFromMemory(pD3DDev, &loader.GetData()[0], loader.GetSize(), &pTexture_, &pTextureView_);

	if(FAILED(hr)){
		pTexture_ = nullptr;
		pTextureView_ = nullptr;
		SetDebugMessage("TexturLoadError! [%s] �� [%s] ����ǂݍ��߂܂���ł���\n", pFileName, pArchiveFileName);
	}

	return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
// �X�g���[�W����摜��ǂݍ���
// �����@�F�t�@�C����(const char* pFileName)
// �@�@�@�@���ɕ������鐔(unsigned int DivX),�c�ɕ������鐔(unsigned int DivY)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Texture::LoadImageFromStorage(const char* pFileName, unsigned int divX, unsigned int divY)
{
	if(pTexture_){ return false; }	// ���ɓǂݍ��ݍς݂Ȃ�I��

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// ���������i�[����
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// �X�g���[�W����摜��ǂݍ���
	Texture& tex = StorageManager::Instance().GetTexture(pFileName);
	pTexture_ = tex.pTexture_;
	pTextureView_ = tex.pTextureView_;

	if(pTexture_ && pTextureView_){ bStorage_ = true; }	// �X�g���[�W�g�p�t���O���I����
	else{
		pTexture_ = nullptr;
		pTextureView_ = nullptr;
		SetDebugMessage("TexturLoadError! [%s] ���X�g���[�W����ǂݍ��߂܂���ł����B\n", pFileName);
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------
// �摜�̔j��
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Texture::UnLoad()
{
	if(!pTexture_){ return; }

	if(vecTexHolderPtr_.size() > 0){
		for(auto it = vecTexHolderPtr_.begin(), itEnd = vecTexHolderPtr_.end(); it != itEnd; ++it){
			if(*it){
				(*it)->pTex_ = nullptr;
			}
		}
	}

	if(!bStorage_){
		SafeRelease(pTextureView_);
		SafeRelease(pTexture_);
	}

	ZeroMemory(fileName_, ArraySize(fileName_));
	pTexture_ = nullptr;
	pTextureView_ = nullptr;
	divU_ = 1.0f;
	divV_ = 1.0f;
	bStorage_ = false;
}