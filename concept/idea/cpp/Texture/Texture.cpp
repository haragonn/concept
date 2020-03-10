/*==============================================================================
	[Texture.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
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
// コンストラクタ
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
// デストラクタ
//------------------------------------------------------------------------------
Texture::~Texture()
{
	UnLoad();
}

//------------------------------------------------------------------------------
// 画像の読み込み
// 引数　：ファイル名(const char* pFileName)
// 　　　　横に分割する数(unsigned int divX),縦に分割する数(unsigned int divY)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Texture::LoadImageFromFile(const char* pFileName, unsigned int divX, unsigned int divY)
{
	auto pD3DDev = GraphicManager::Instance().GetDevicePtr();

	if(!pD3DDev || pTextureView_){ return false; }	// デバイスがないか既に読み込み済みなら終了

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// 分割情報を格納する
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// ファイルパスの変換
	setlocale(LC_ALL, "japanese");
	wchar_t tFileNamePtr[MAX_PATH];
	size_t size;
	mbstowcs_s(&size, tFileNamePtr, MAX_PATH, pFileName, _TRUNCATE);

	// 拡張子
	char ms[MAX_PATH];
	wcstombs_s(&size, ms, MAX_PATH, tFileNamePtr, _TRUNCATE);

	char* extension = strstr(ms, ".");
	if(!extension){
		SetDebugMessage("TexturLoadError! [%s] に拡張子が存在しません。\n", pFileName);
		return false;
	}

	// 画像の読み込み
	if(strcmp(extension, ".tga") == 0 || strcmp(extension, ".TGA") == 0) {
		TexMetadata meta;

		if(FAILED(GetMetadataFromTGAFile(tFileNamePtr, meta))){
			SetDebugMessage("TexturLoadError! [%s] をファイルから読み込めませんでした。\n", pFileName);
			return false;
		}

		std::unique_ptr<ScratchImage> image(new ScratchImage);

		if(FAILED(LoadFromTGAFile(tFileNamePtr, &meta, *image))){
			SetDebugMessage("TexturLoadError! [%s] をファイルから読み込めませんでした。\n", pFileName);
			return false;
		}

		if(FAILED(CreateShaderResourceView(pD3DDev, image->GetImages(), image->GetImageCount(), meta, &pTextureView_))){
			SetDebugMessage("TexturLoadError! [%s] からリソースビューを作成できませんでした。\n", pFileName);
			return false;
		}
	} else{
		TexMetadata meta;

		if(FAILED(GetMetadataFromWICFile(tFileNamePtr, 0, meta))){
			SetDebugMessage("TexturLoadError! [%s] をファイルから読み込めませんでした。\n", pFileName);
			return false;
		}

		std::unique_ptr<ScratchImage> image(new ScratchImage);

		if(FAILED(LoadFromWICFile(tFileNamePtr, 0, &meta, *image))){
			SetDebugMessage("TexturLoadError! [%s] をファイルから読み込めませんでした。\n", pFileName);
			return false;
		}

		if(FAILED(CreateShaderResourceView(pD3DDev, image->GetImages(), image->GetImageCount(), meta, &pTextureView_))){
			pTextureView_ = nullptr;
			SetDebugMessage("TexturLoadError! [%s] からリソースビューを作成できませんでした。\n", pFileName);
			return false;
		}
	}
	
	return true;
}

bool Texture::LoadImageFromArchiveFile(const char * pArchiveFileName, const char * pFileName, unsigned int divX, unsigned int divY)
{
	auto pD3DDev = GraphicManager::Instance().GetDevicePtr();

	if(!pD3DDev || pTextureView_){ return false; }	// デバイスがないか既に読み込み済みなら終了

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// 分割情報を格納する
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// 画像の読み込み
	ArchiveLoader loader;
	if(!loader.Load(pArchiveFileName, pFileName)){
		SetDebugMessage("ArchiveLoadError! [%s] をファイルから読み込めませんでした\n", pArchiveFileName);
		return false;
	}

	char ms[MAX_PATH];
	strcpy_s(ms, pFileName);
	char* extension = strstr(ms, ".");
	if(!extension){
		SetDebugMessage("TexturLoadError! [%s] に拡張子が存在しません。\n", pFileName);
		return false;
	}

	// 画像の読み込み
	if(strcmp(extension, ".tga") == 0 || strcmp(extension, ".TGA") == 0) {
		TexMetadata meta;

		if(FAILED(GetMetadataFromTGAMemory(&loader.GetData()[0], loader.GetSize(), meta))){
			SetDebugMessage("TexturLoadError! [%s] を [%s] から読み込めませんでした。\n", pFileName, pArchiveFileName);
			return false;
		}

		std::unique_ptr<ScratchImage> image(new ScratchImage);

		if(FAILED(LoadFromTGAMemory(&loader.GetData()[0], loader.GetSize(), &meta, *image))){
			SetDebugMessage("TexturLoadError! [%s] を [%s] から読み込めませんでした。\n", pFileName, pArchiveFileName);
			return false;
		}

		if(FAILED(CreateShaderResourceView(pD3DDev, image->GetImages(), image->GetImageCount(), meta, &pTextureView_))){
			pTextureView_ = nullptr;
			SetDebugMessage("TexturLoadError! [%s] からリソースビューを作成できませんでした。\n", pFileName);
			return false;
		}
	} else{
		TexMetadata meta;

		if(FAILED(GetMetadataFromWICMemory(&loader.GetData()[0], loader.GetSize(), 0, meta))){
			SetDebugMessage("TexturLoadError! [%s] を [%s] から読み込めませんでした。\n", pFileName, pArchiveFileName);
			return false;
		}

		std::unique_ptr<ScratchImage> image(new ScratchImage);

		if(FAILED(LoadFromWICMemory(&loader.GetData()[0], loader.GetSize(), 0, &meta, *image))){
			SetDebugMessage("TexturLoadError! [%s] を [%s] から読み込めませんでした。\n", pFileName, pArchiveFileName);
			return false;
		}

		if(FAILED(CreateShaderResourceView(pD3DDev, image->GetImages(), image->GetImageCount(), meta, &pTextureView_))){
			pTextureView_ = nullptr;
			SetDebugMessage("TexturLoadError! [%s] からリソースビューを作成できませんでした。\n", pFileName);
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------
// ストレージから画像を読み込む
// 引数　：ファイル名(const char* pFileName)
// 　　　　横に分割する数(unsigned int DivX),縦に分割する数(unsigned int DivY)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Texture::LoadImageFromStorage(const char* pFileName, unsigned int divX, unsigned int divY)
{
	if(pTextureView_){ return false; }	// 既に読み込み済みなら終了

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// 分割情報を格納する
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// ストレージから画像を読み込む
	Texture& tex = StorageManager::Instance().GetTexture(pFileName);
	pTextureView_ = tex.pTextureView_;

	if(pTextureView_){ bStorage_ = true; }	// ストレージ使用フラグをオンに
	else{
		pTextureView_ = nullptr;
		SetDebugMessage("TexturLoadError! [%s] をストレージから読み込めませんでした。\n", pFileName);
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// 画像の破棄
// 引数　：なし
// 戻り値：なし
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