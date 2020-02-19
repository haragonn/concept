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
#include "WICTextureLoader.h"
#include <d3d11.h>
#include <locale.h>

using namespace std;
using namespace DirectX;

//------------------------------------------------------------------------------
// コンストラクタ
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
// デストラクタ
//------------------------------------------------------------------------------
Texture::~Texture()
{
	// 終了処理
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
	ID3D11Device* pD3DDev = GraphicManager::Instance().GetDevicePtr();
	if(!pD3DDev || pTexture_){ return false; }	// デバイスがないか既に読み込み済みなら終了

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// 分割情報を格納する
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// 画像の読み込み
	setlocale(LC_ALL, "japanese");
	wchar_t pTemp[MAX_PATH];
	size_t size;
	mbstowcs_s(&size, pTemp, MAX_PATH, pFileName, _TRUNCATE);

	HRESULT hr = CreateWICTextureFromFile(pD3DDev, pTemp, &pTexture_, &pTextureView_);

	if(FAILED(hr)){
		pTexture_ = nullptr;
		pTextureView_ = nullptr;
		SetDebugMessage("TexturLoadError! [%s] をファイルから読み込めませんでした\n", pFileName);
	}
	
	return SUCCEEDED(hr);
}

bool Texture::LoadImageFromArchiveFile(const char * pArchiveFileName, const char * pFileName, unsigned int divX, unsigned int divY)
{
	ID3D11Device* pD3DDev = GraphicManager::Instance().GetDevicePtr();
	if(!pD3DDev || pTexture_){ return false; }	// デバイスがないか既に読み込み済みなら終了

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// 分割情報を格納する
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// 画像の読み込み
	ArchiveLoader loader;
	if(!loader.Load(pArchiveFileName, pFileName)){
		SetDebugMessage("TexturLoadError! [%s] を [%s] から読み込めませんでした\n", pFileName, pArchiveFileName);
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
		SetDebugMessage("TexturLoadError! [%s] を [%s] から読み込めませんでした\n", pFileName, pArchiveFileName);
	}

	return SUCCEEDED(hr);
}

//------------------------------------------------------------------------------
// ストレージから画像を読み込む
// 引数　：ファイル名(const char* pFileName)
// 　　　　横に分割する数(unsigned int DivX),縦に分割する数(unsigned int DivY)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Texture::LoadImageFromStorage(const char* pFileName, unsigned int divX, unsigned int divY)
{
	if(pTexture_){ return false; }	// 既に読み込み済みなら終了

	ZeroMemory(fileName_, ArraySize(fileName_));
	memcpy(fileName_, pFileName, 255);

	// 分割情報を格納する
	if(divX > 1){ divU_ = 1.0f / divX; }
	if(divY > 1){ divV_ = 1.0f / divY; }

	// ストレージから画像を読み込む
	Texture& tex = StorageManager::Instance().GetTexture(pFileName);
	pTexture_ = tex.pTexture_;
	pTextureView_ = tex.pTextureView_;

	if(pTexture_ && pTextureView_){ bStorage_ = true; }	// ストレージ使用フラグをオンに
	else{
		pTexture_ = nullptr;
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