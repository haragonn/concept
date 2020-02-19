/*==============================================================================
	[Actor2DManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/2D/Actor2DManager.h"
#include "../../h/2D/Actor2D.h"

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Actor2DManager::Actor2DManager() :
	pListBegin_(nullptr),
	pListEnd_(nullptr)
{}

//------------------------------------------------------------------------------
// 登録
// 引数　：登録するActor2D(Actor2D* pActor2D)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2DManager::Register(Actor2D* pActor2D)
{
	if(!pActor2D || pActor2D->bRegistered_){ return; }	// NULLチェック,登録済みチェック
	if(!pListBegin_){ pListBegin_ = pActor2D; }	// 先頭がなければ先頭に
	else{
		pListEnd_->pNext_ = pActor2D;	// 現在の最後尾の次のポインタとする
		pActor2D->pPrev_ = pListEnd_;	// 前のポインタに現在の最後尾を入れる
	}
	pListEnd_ = pActor2D;			// 最後尾とする
	pActor2D->bRegistered_ = true;	// 登録済みに
}

//------------------------------------------------------------------------------
// 登録解除
// 引数　：登録解除するActor2D(Actor2D * pActor2D)
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2DManager::UnRegister(Actor2D* pActor2D)
{
	if(!pActor2D || !pActor2D->bRegistered_){ return; }	// NULLチェック,削除済みチェック
	if(pActor2D == pListBegin_){	// 先頭のポインタならば
		pListBegin_ = pActor2D->pNext_;	// 次のポインタを先頭とする
		if(pActor2D->pNext_){ pActor2D->pNext_->pPrev_ = nullptr; }	// 次のポインタが存在するならばそのポインタの前のポインタをNULLとする
	} else if(pActor2D == pListEnd_){	// 最後尾ならば
		pListEnd_ = pActor2D->pPrev_;	// 前のポインタを最後尾とする
		if(pActor2D->pPrev_){ pActor2D->pPrev_->pNext_ = nullptr; }	// 最後尾の次のポインタはNULLとする
	} else{
		pActor2D->pPrev_->pNext_ = pActor2D->pNext_;	// 前のポインタの次のポインタを更新する
		if(pActor2D->pNext_){ pActor2D->pNext_->pPrev_ = pActor2D->pPrev_; }	// 次のポインタが存在するならばそのポインタの前のポインタを更新する
	}
	pActor2D->bRegistered_ = false;	// 削除済みに
}

//------------------------------------------------------------------------------
// 座標の保持
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Actor2DManager::WriteDownPosition()
{
	if(pListBegin_){	// 管理していたら
		// リストのActor2Dに座標の保持をさせる
		for(Actor2D* pAct = pListBegin_; pAct != nullptr; pAct = pAct->pNext_){
			pAct->WriteDownPosition();
		}
	}
}
