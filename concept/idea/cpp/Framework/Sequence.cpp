/*==============================================================================
	[Sequence.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Framework/Sequence.h"
#include "../../h/Framework/Scene.h"
#include "../../h/Framework/AsyncLoadScene.h"
#include "../../h/Storage/StorageManager.h"
#include "../../../eidos/h/eidosStorage/eidosStorageManager.h"
#include "../../h/Network/NetworkManager.h"

#include <exception>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//------------------------------------------------------------------------------
// 非同期読み込みスレッド
//------------------------------------------------------------------------------
DWORD WINAPI AsyncLoadThread(void* vp)
{
	Sequence* psq = (Sequence*)vp;

	psq->AsyncLoad();
	psq->bLoadCompleted_ = true;

	ExitThread(0);
}

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Sequence::Sequence() :
	pScene_(nullptr),
	hAsyncLoadThread_(nullptr),
	bLoadCompleted_(false){}

//------------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------------
Sequence::~Sequence()
{
	UnInit();
}

//------------------------------------------------------------------------------
// 初期化
// 引数　：開始するシーンのポインター(Scene* pInitScene)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Sequence::Init(Scene* pInitScene)
{
	if(!pInitScene){ return false; }	// NULLチェック

	if(!pScene_){	// 既にシーンを持っていなければ
		pScene_ = pInitScene;	// シーンを登録
		pScene_->Init();

		return true;
	}

	// シーンの破棄
	delete pInitScene;

	return false;
}

//------------------------------------------------------------------------------
// 終了処理
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Sequence::UnInit()
{
	// スレッドの破棄
	if(hAsyncLoadThread_){
#ifdef _DEBUG
		// デバッグなら3分たったらで殺しちゃえ
		if(WaitForSingleObject(hAsyncLoadThread_, 3 * 60 * 1000) == WAIT_TIMEOUT){
			MessageBox(NULL, "AsyncLoadThreadが終了しません。強制終了します。", NULL, NULL);
			TerminateThread(hAsyncLoadThread_ , FALSE);
		}
#else
		WaitForSingleObject(hAsyncLoadThread_, INFINITE);
#endif
		CloseHandle(hAsyncLoadThread_);
		hAsyncLoadThread_ = nullptr;
	}

	// シーンの破棄
	if(pScene_){
		pScene_->UnInit();
		delete pScene_;
		pScene_ = nullptr;
	}
}

//------------------------------------------------------------------------------
// 更新
// 引数　：なし
// 戻り値：アプリケーションを続行するかどうか
//------------------------------------------------------------------------------
int Sequence::Update(bool bRedy)
{
	if(pScene_){	// NULLチェック
		if(pScene_->bAsync_){
			if(!bLoadCompleted_ && !hAsyncLoadThread_){
				hAsyncLoadThread_ = CreateThread(NULL, 0, AsyncLoadThread, this, 0, NULL);	// 非同期読み込みスレッドの作成
			} else{
				// 非同期読み込みスレッドの監視
				DWORD ExitCode;
				GetExitCodeThread(hAsyncLoadThread_, &ExitCode);
				if(ExitCode != STILL_ACTIVE){
					CloseHandle(hAsyncLoadThread_);
					hAsyncLoadThread_ = nullptr;
				}
			}
		} else{ bLoadCompleted_ = true; }

		Scene* pNext = pScene_->Update();	// シーンのアップデートから次のシーンを受け取る

		NetworkManager::Instance().WritingEnd();	// ネットワーク書き込み

		if(pNext == pScene_){ return 0; }	// 変わらなければそのまま

		// 非同期読み込みスレッドの破棄
		if(hAsyncLoadThread_){
			if(WaitForSingleObject(hAsyncLoadThread_, INFINITE) == WAIT_TIMEOUT){ throw std::exception(); }
			CloseHandle(hAsyncLoadThread_);
			hAsyncLoadThread_ = nullptr;
		}

		// 異なるシーンを受け取ったら前のシーンを破棄してシーンを切り替える
		pScene_->UnInit();

		NetworkManager::Instance().WritingEnd();

		delete pScene_;

		// ストレージの破棄要請チェック
		if(StorageManager::Instance().IsUnLoadAllRequest()){
			StorageManager::Instance().UnLoadAll();
		} else{
			if(StorageManager::Instance().GetUnLoadImageRequestSize()){
				StorageManager::Instance().UnLoadImage();
			}
			if(StorageManager::Instance().GetUnLoadWaveRequestSize()){
				StorageManager::Instance().UnLoadWave();
			}
		}

		if(eidosStorageManager::Instance().IsUnLoadAllRequest()){
			eidosStorageManager::Instance().UnLoadAll();
		} else{
			if(eidosStorageManager::Instance().GetUnLoadObjModelRequestSize()){
				eidosStorageManager::Instance().UnLoadObjModel();
			}
			if(eidosStorageManager::Instance().GetUnLoadPmdModelRequestSize()){
				eidosStorageManager::Instance().UnLoadPmdModel();
			}
		}

		// シーン遷移
		if(pNext && bRedy){	// 次のシーンがあれば
			pScene_ = pNext;

			if(pScene_->bAsync_){ bLoadCompleted_ = false; }

			pScene_->Init();

			return 1;	// 続行
		}
		pScene_ = nullptr;
	}

	return -1;	// 中断
}

//------------------------------------------------------------------------------
// 描画
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Sequence::Draw()
{
	if(pScene_){ pScene_->Draw(); }	// NULLチェック
}

//------------------------------------------------------------------------------
// 非同期読み込み
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Sequence::AsyncLoad()
{
	if(pScene_ && pScene_->bAsync_ && dynamic_cast<AsyncLoadScene*>(pScene_)->bLoadCompleted_){ return; }
	if(pScene_ && pScene_->bAsync_){ dynamic_cast<AsyncLoadScene*>(pScene_)->Load(); }
	if(pScene_ && pScene_->bAsync_){ dynamic_cast<AsyncLoadScene*>(pScene_)->bLoadCompleted_ = true; }
}