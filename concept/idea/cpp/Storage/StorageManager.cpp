/*==============================================================================
	[StorageManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Storage/StorageManager.h"
#include "../../h/Texture/Texture.h"
#include "../../h/Sound/Sound.h"
#include "../../h/Utility/ideaUtility.h"

using namespace std;

Texture StorageManager::s_NullTexture_;
Sound StorageManager::s_NullSound_;

StorageManager::StorageManager()
	: bUnLoadAll_(false)	
{
	unordered_map<const char*, Texture*>().swap(umTexturePtr_);
	vector<const char*>().swap(vecUnLoadImageList_);

	unordered_map<const char*, Sound*>().swap(umSoundPtr_);
	unordered_map<const char*, unsigned int>().swap(umSoundSize_);
	unordered_map<const char*, unsigned int>().swap(umSoundIndex_);
	vector<const char*>().swap(vecUnLoadWaveList_);
}

//------------------------------------------------------------------------------
// 画像の読み込み
// 引数　：ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool StorageManager::LoadImageFromFile(const char * pFileName)
{
	if(HasKeyTexture(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	Texture* pTex = new Texture;
	bool bRes = pTex->LoadImageFromFile(pFileName);

	if(bRes){
		umTexturePtr_.insert(pair<const char*, Texture*>{ pFileName, pTex });
	} else{
		SafeDelete(pTex);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// アーカイブファイルから画像を読み込む
// 引数　：アーカイブファイル名(const char* pArchiveFileName),
// 　　　　ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool StorageManager::LoadImageFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	if(HasKeyTexture(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	Texture* pTex = new Texture;
	bool bRes = pTex->LoadImageFromArchiveFile(pArchiveFileName, pFileName);

	if(bRes){
		umTexturePtr_.insert(pair<const char*, Texture*>{ pFileName, pTex });
	} else{
		SafeDelete(pTex);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// 画像の取得
// 引数　：ファイル名(const char* pFileName)
// 戻り値：テクスチャー
//------------------------------------------------------------------------------
Texture& StorageManager::GetTexture(const char * pFileName)
{
	if(!HasKeyTexture(pFileName)
		|| !umTexturePtr_.size()){
		SetDebugMessage("TexturLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);
		if(!LoadImageFromFile(pFileName)){
			return s_NullTexture_;
		}
	}

	Texture* pTex = umTexturePtr_[pFileName];

	if(!pTex){ return s_NullTexture_; }

	return *pTex;	// テクスチャーインターフェイスのポインターを返す
}

//------------------------------------------------------------------------------
// 指定したファイル名と関連付けられた画像が存在するか
// 引数　：ファイル名(const char* pFileName)
// 戻り値：存在するか
//------------------------------------------------------------------------------
bool StorageManager::HasKeyTexture(const char * pFileName)
{
	if(!umTexturePtr_.size()){ return false; }
	return (umTexturePtr_.find(pFileName) != umTexturePtr_.end());	// 検索
}

//------------------------------------------------------------------------------
// waveファイルの読み込み
// 引数　：ファイル名(const char* pFileName),サウンド数(unsigned int soundSize)
// 戻り値：成否
//------------------------------------------------------------------------------
bool StorageManager::LoadWaveFromFile(const char* pFileName, unsigned int soundSize)
{
	if(HasKeyWave(pFileName)
		|| soundSize == 0){ return false; }
	
	umSoundSize_.insert(pair<const char*, unsigned int >{ pFileName, soundSize });
	umSoundIndex_.insert(pair<const char*, unsigned int >{ pFileName, 0 });

	bool bRes;
	Sound* pSnd = new Sound[soundSize];
	for(unsigned int i = 0U; i < soundSize; ++i){
		bRes = pSnd[i].LoadWaveFromFile(pFileName);
	}

	if(bRes){
		umSoundPtr_.insert(pair<const char*, Sound*>{ pFileName, pSnd });
	} else{
		SafeDeleteArray(pSnd);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// アーカイブファイルからwaveファイルを読み込む
// 引数　：アーカイブファイル名(const char* pArchiveFileName),
// 　　　　ファイル名(const char* pFileName),サウンド数(unsigned int soundNum)
// 戻り値：成否
//------------------------------------------------------------------------------
bool StorageManager::LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize)
{
	if(HasKeyWave(pFileName)
		|| soundSize == 0){ return false; }

	umSoundSize_.insert(pair<const char*, unsigned int >{ pFileName, soundSize });
	umSoundIndex_.insert(pair<const char*, unsigned int >{ pFileName, 0 });

	bool bRes;
	Sound* pSnd = new Sound[soundSize];
	for(unsigned int i = 0U; i < soundSize; ++i){
		bRes = pSnd[i].LoadWaveFromArchiveFile(pArchiveFileName, pFileName);
	}

	if(bRes){
		umSoundPtr_.insert(pair<const char*, Sound*>{ pFileName, pSnd });
	} else{
		SafeDeleteArray(pSnd);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// サウンドの取得
// 引数　：ファイル名(const char* pFileName)
// 戻り値：サウンド
//------------------------------------------------------------------------------
Sound& StorageManager::GetSound(const char * pFileName)
{
	if(!HasKeyWave(pFileName)
		|| !umSoundPtr_.size()){
		SetDebugMessage("WaveLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);
		if(!LoadWaveFromFile(pFileName)){
			return s_NullSound_;
		}
	}

	Sound* pSnd = umSoundPtr_[pFileName];
	if(!pSnd){ return s_NullSound_; }

	Sound& snd = pSnd[umSoundIndex_[pFileName] % umSoundSize_[pFileName]];
	umSoundIndex_[pFileName] = umSoundIndex_[pFileName] + 1;

	return snd;
}

//------------------------------------------------------------------------------
// 指定したファイル名と関連付けられたwaveデータが存在するか
// 引数　：ファイル名(const char* pFileName)
// 戻り値：存在するか
//------------------------------------------------------------------------------
bool StorageManager::HasKeyWave(const char * pFileName)
{
	if(!umSoundPtr_.size()){ return false; }
	return (umSoundPtr_.find(pFileName) != umSoundPtr_.end());	// 検索
}


void StorageManager::SetUnLoadImageRequest(const char * pFileName)
{
	vector<const char*>::iterator it = vecUnLoadImageList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadImageList_.end();

	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadImageList_.push_back(pFileName);
}

void StorageManager::UnLoadImage()
{
	if(umTexturePtr_.size()){
		// イテレータを使って解放
		vector<const char*>::iterator it = vecUnLoadImageList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadImageList_.end();
		unordered_map<const char*, Texture*>::iterator itTexEnd = umTexturePtr_.end();
		for(; it != itEnd; ++it){
			if(umTexturePtr_.find(*it) != itTexEnd){
				unordered_map<const char*, Texture*>::iterator itTex = umTexturePtr_.find(*it);
				if(itTex->second){
					itTex->second->UnLoad();
					delete itTex->second;
					itTex->second = nullptr;
				}
				umTexturePtr_.erase(itTex);
			}
		}
	}
	vector<const char*>().swap(vecUnLoadImageList_);
}

void StorageManager::SetUnLoadWaveRequest(const char * pFileName)
{
	vector<const char*>::iterator it = vecUnLoadWaveList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadWaveList_.end();
	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadWaveList_.push_back(pFileName);
}

void StorageManager::UnLoadWave()
{
	if(umSoundPtr_.size()){
		// イテレータを使って解放
		vector<const char*>::iterator it = vecUnLoadWaveList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadWaveList_.end();
		unordered_map<const char*, Sound*>::iterator itSndEnd = umSoundPtr_.end();
		for(; it != itEnd; ++it){
			if(umSoundPtr_.find(*it) != itSndEnd){
				unordered_map<const char*, Sound*>::iterator itSnd = umSoundPtr_.find(*it);
				if(itSnd->second){
					for(unsigned int i = 0U; i < umSoundSize_[itSnd->first]; ++i){
						itSnd->second[i].UnLoad();
					}
					delete[] itSnd->second;
					itSnd->second = nullptr;
				}
				umSoundPtr_.erase(itSnd);
			}
		}
	}

	vector<const char*>().swap(vecUnLoadWaveList_);
}

//------------------------------------------------------------------------------
// 全ての画像とサウンドの破棄
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void StorageManager::UnLoadAll()
{
	// イテレータを使って解放
	if(umTexturePtr_.size()){
		unordered_map<const char*, Texture*>::iterator itTex = umTexturePtr_.begin();
		unordered_map<const char*, Texture*>::iterator itTexEnd = umTexturePtr_.end();

		for(; itTex != itTexEnd; ++itTex){
			if(itTex->second){
				itTex->second->UnLoad();
				delete itTex->second;
				itTex->second = nullptr;
			}
		}
	}

	unordered_map<const char*, Texture*>().swap(umTexturePtr_);

	if(umSoundPtr_.size()){
		unordered_map<const char*, Sound*>::iterator itSnd = umSoundPtr_.begin();
		unordered_map<const char*, Sound*>::iterator itSndEnd = umSoundPtr_.end();

		for(; itSnd != itSndEnd; ++itSnd){
			if(itSnd->second){
				for(unsigned int i = 0U; i < umSoundSize_[itSnd->first]; ++i){
					itSnd->second[i].UnLoad();
				}
				delete[] itSnd->second;
				itSnd->second = nullptr;
			}
		}
	}

	unordered_map<const char*, Sound*>().swap(umSoundPtr_);
	unordered_map<const char*, unsigned int>().swap(umSoundSize_);
	unordered_map<const char*, unsigned int>().swap(umSoundIndex_);

	vector<const char*>().swap(vecUnLoadImageList_);
	vector<const char*>().swap(vecUnLoadWaveList_);

	bUnLoadAll_ = false;
}