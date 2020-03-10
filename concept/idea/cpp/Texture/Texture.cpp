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
#include <d3d11.h>
#include <locale.h>
#include "DirectXTex.h"

#ifdef _DEBUG
#pragma comment(lib, "DirectXTex_d.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#endif

using namespace std;
using namespace DirectX;

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
Texture::Texture():
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
	auto pD3DDev = GraphicManager::Instance().GetDevicePtr();

	if(!pD3DDev || pTextureView_){ return false; }	// �f�o�C�X���Ȃ������ɓǂݍ��ݍς݂Ȃ�I��

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// ���������i�[����
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// �t�@�C���p�X�̕ϊ�
	setlocale(LC_ALL, "japanese");
	wchar_t tFileNamePtr[MAX_PATH];
	size_t size;
	mbstowcs_s(&size, tFileNamePtr, MAX_PATH, pFileName, _TRUNCATE);

	// �g���q
	char ms[MAX_PATH];
	wcstombs_s(&size, ms, MAX_PATH, tFileNamePtr, _TRUNCATE);

	char* extension = strstr(ms, ".");
	if(!extension){
		SetDebugMessage("TexturLoadError! [%s] �Ɋg���q�����݂��܂���B\n", pFileName);
		return false;
	}

	// �摜�̓ǂݍ���
	if(strcmp(extension, ".tga") == 0 || strcmp(extension, ".TGA") == 0) {
		TexMetadata meta;

		if(FAILED(GetMetadataFromTGAFile(tFileNamePtr, meta))){
			SetDebugMessage("TexturLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł����B\n", pFileName);
			return false;
		}

		std::unique_ptr<ScratchImage> image(new ScratchImage);

		if(FAILED(LoadFromTGAFile(tFileNamePtr, &meta, *image))){
			SetDebugMessage("TexturLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł����B\n", pFileName);
			return false;
		}

		if(FAILED(CreateShaderResourceView(pD3DDev, image->GetImages(), image->GetImageCount(), meta, &pTextureView_))){
			SetDebugMessage("TexturLoadError! [%s] ���烊�\�[�X�r���[���쐬�ł��܂���ł����B\n", pFileName);
			return false;
		}
	} else{
		TexMetadata meta;

		if(FAILED(GetMetadataFromWICFile(tFileNamePtr, 0, meta))){
			SetDebugMessage("TexturLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł����B\n", pFileName);
			return false;
		}

		std::unique_ptr<ScratchImage> image(new ScratchImage);

		if(FAILED(LoadFromWICFile(tFileNamePtr, 0, &meta, *image))){
			SetDebugMessage("TexturLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł����B\n", pFileName);
			return false;
		}

		if(FAILED(CreateShaderResourceView(pD3DDev, image->GetImages(), image->GetImageCount(), meta, &pTextureView_))){
			pTextureView_ = nullptr;
			SetDebugMessage("TexturLoadError! [%s] ���烊�\�[�X�r���[���쐬�ł��܂���ł����B\n", pFileName);
			return false;
		}
	}
	
	return true;
}

bool Texture::LoadImageFromArchiveFile(const char * pArchiveFileName, const char * pFileName, unsigned int divX, unsigned int divY)
{
	auto pD3DDev = GraphicManager::Instance().GetDevicePtr();

	if(!pD3DDev || pTextureView_){ return false; }	// �f�o�C�X���Ȃ������ɓǂݍ��ݍς݂Ȃ�I��

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// ���������i�[����
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// �摜�̓ǂݍ���
	ArchiveLoader loader;
	if(!loader.Load(pArchiveFileName, pFileName)){
		SetDebugMessage("ArchiveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pArchiveFileName);
		return false;
	}

	char ms[MAX_PATH];
	strcpy_s(ms, pFileName);
	char* extension = strstr(ms, ".");
	if(!extension){
		SetDebugMessage("TexturLoadError! [%s] �Ɋg���q�����݂��܂���B\n", pFileName);
		return false;
	}

	// �摜�̓ǂݍ���
	if(strcmp(extension, ".tga") == 0 || strcmp(extension, ".TGA") == 0) {
		TexMetadata meta;

		if(FAILED(GetMetadataFromTGAMemory(&loader.GetData()[0], loader.GetSize(), meta))){
			SetDebugMessage("TexturLoadError! [%s] �� [%s] ����ǂݍ��߂܂���ł����B\n", pFileName, pArchiveFileName);
			return false;
		}

		std::unique_ptr<ScratchImage> image(new ScratchImage);

		if(FAILED(LoadFromTGAMemory(&loader.GetData()[0], loader.GetSize(), &meta, *image))){
			SetDebugMessage("TexturLoadError! [%s] �� [%s] ����ǂݍ��߂܂���ł����B\n", pFileName, pArchiveFileName);
			return false;
		}

		if(FAILED(CreateShaderResourceView(pD3DDev, image->GetImages(), image->GetImageCount(), meta, &pTextureView_))){
			pTextureView_ = nullptr;
			SetDebugMessage("TexturLoadError! [%s] ���烊�\�[�X�r���[���쐬�ł��܂���ł����B\n", pFileName);
			return false;
		}
	} else{
		TexMetadata meta;

		if(FAILED(GetMetadataFromWICMemory(&loader.GetData()[0], loader.GetSize(), 0, meta))){
			SetDebugMessage("TexturLoadError! [%s] �� [%s] ����ǂݍ��߂܂���ł����B\n", pFileName, pArchiveFileName);
			return false;
		}

		std::unique_ptr<ScratchImage> image(new ScratchImage);

		if(FAILED(LoadFromWICMemory(&loader.GetData()[0], loader.GetSize(), 0, &meta, *image))){
			SetDebugMessage("TexturLoadError! [%s] �� [%s] ����ǂݍ��߂܂���ł����B\n", pFileName, pArchiveFileName);
			return false;
		}

		if(FAILED(CreateShaderResourceView(pD3DDev, image->GetImages(), image->GetImageCount(), meta, &pTextureView_))){
			pTextureView_ = nullptr;
			SetDebugMessage("TexturLoadError! [%s] ���烊�\�[�X�r���[���쐬�ł��܂���ł����B\n", pFileName);
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
// �X�g���[�W����摜��ǂݍ���
// �����@�F�t�@�C����(const char* pFileName)
// �@�@�@�@���ɕ������鐔(unsigned int DivX),�c�ɕ������鐔(unsigned int DivY)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Texture::LoadImageFromStorage(const char* pFileName, unsigned int divX, unsigned int divY)
{
	if(pTextureView_){ return false; }	// ���ɓǂݍ��ݍς݂Ȃ�I��

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// ���������i�[����
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// �X�g���[�W����摜��ǂݍ���
	Texture& tex = StorageManager::Instance().GetTexture(pFileName);
	pTextureView_ = tex.pTextureView_;

	if(pTextureView_){ bStorage_ = true; }	// �X�g���[�W�g�p�t���O���I����
	else{
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
	if(!pTextureView_){ return; }

	if(vecTexHolderPtr_.size() > 0){
		for(auto it = vecTexHolderPtr_.begin(), itEnd = vecTexHolderPtr_.end(); it != itEnd; ++it){
			if(*it){
				(*it)->pTex_ = nullptr;
			}
		}
	}
	vector<TextureHolder*>().swap(vecTexHolderPtr_);

	if(!bStorage_){
		SafeRelease(pTextureView_);
	}

	ZeroMemory(fileName_, ArraySize(fileName_));

	divU_ = 1.0f;
	divV_ = 1.0f;

	bStorage_ = false;
}