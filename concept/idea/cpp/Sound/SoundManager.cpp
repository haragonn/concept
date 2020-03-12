/*==============================================================================
	[SoundManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Sound/SoundManager.h"
#include "../../h/Sound/Sound.h"
#include "../../h/Utility/ideaUtility.h"

#pragma comment(lib, "xaudio2.lib")

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
SoundManager::SoundManager() :
	pXAudio2_(nullptr),
	pMasteringVoice_(nullptr),
	bCom_(false),
	pListBegin_(nullptr),
	pListEnd_(nullptr)
{}

//------------------------------------------------------------------------------
// 初期化
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool SoundManager::Init()
{
	// COMライブラリの初期化
	if(FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))){
		bCom_ = false;
		return false;
	}
	bCom_ = true;

	// XAudio2オブジェクトの作成
	if(FAILED(XAudio2Create(&pXAudio2_, 0))){
		CoUninitialize();
		bCom_ = false;
		return false;
	}

	// マスターボイスの作成
	if(FAILED(pXAudio2_->CreateMasteringVoice(&pMasteringVoice_))){
		if(pXAudio2_){
			pXAudio2_->Release();
			pXAudio2_ = nullptr;
		}
		CoUninitialize();
		bCom_ = false;
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// 終了処理
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void SoundManager::UnInit()
{
	// 生き残ってるSoundを撤収
	if(pListBegin_){
		for(Sound* pSnd = pListBegin_; pSnd != nullptr; pSnd = pSnd->pNext_){
			pSnd->UnLoad();
		}
		pListBegin_ = nullptr;
	}

	// 解放
	if(pMasteringVoice_){
		pMasteringVoice_->DestroyVoice();
		pMasteringVoice_ = nullptr;
	}

	SafeRelease(pXAudio2_);

	if(bCom_){
		CoUninitialize();
		bCom_ = false;
	}
}

//------------------------------------------------------------------------------
// ソースボイスの生成
// 引数　：ソースボイス構造体のポインターのポインター(IXAudio2SourceVoice** ppSourceVoice)
// 　　　　フォーマットの構造体のポインター(WAVEFORMATEX* pSourceFormat)
// 戻り値：成否
//------------------------------------------------------------------------------
bool SoundManager::CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, WAVEFORMATEX* pSourceFormat)
{
	if(!pXAudio2_){ return false; }	// 初期化されていなければ失敗

	// ソースボスの生成
	if(FAILED(pXAudio2_->CreateSourceVoice(ppSourceVoice, pSourceFormat))){ return false; }

	return true;
}

void SoundManager::Register(Sound * pSound)
{
	if(!pSound || pSound->bRegistered_){ return; }	// NULLチェック,登録済みチェック

	if(!pListBegin_){ pListBegin_ = pSound; }	// 先頭がなければ先頭に
	else{
		pListEnd_->pNext_ = pSound;	// 現在の最後尾の次のポインタとする
		pSound->pPrev_ = pListEnd_;	// 前のポインタに現在の最後尾を入れる
	}

	pListEnd_ = pSound;				// 最後尾とする

	pSound->bRegistered_ = true;	// 登録済みに
}

void SoundManager::UnRegister(Sound * pSound)
{
	if(!pSound || !pSound->bRegistered_){ return; }	// NULLチェック,削除済みチェック

	if(pSound == pListBegin_){	// 先頭のポインタならば
		pListBegin_ = pSound->pNext_;	// 次のポインタを先頭とする
		if(pSound->pNext_){ pSound->pNext_->pPrev_ = nullptr; }	// 次のポインタが存在するならばそのポインタの前のポインタをNULLとする
	}else if(pSound == pListEnd_){	// 最後尾ならば
		pListEnd_ = pSound->pPrev_;	// 前のポインタを最後尾とする
		if(pSound->pPrev_){ pSound->pPrev_->pNext_ = nullptr; }	// 最後尾の次のポインタはNULLとする
	}else{
		pSound->pPrev_->pNext_ = pSound->pNext_;	// 前のポインタの次のポインタを更新する
		if(pSound->pNext_){ pSound->pNext_->pPrev_ = pSound->pPrev_; }	// 次のポインタが存在するならばそのポインタの前のポインタを更新する
	}

	pSound->bRegistered_ = false;	// 削除済みに
}
