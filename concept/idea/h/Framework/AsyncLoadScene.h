/*==============================================================================
	[AsyncLoadScene.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ASYNCLOADSCENE_H
#define INCLUDE_IDEA_ASYNCLOADSCENE_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Scene.h"
#include <atomic>

//------------------------------------------------------------------------------
// クラス名　：AsyncScene
// クラス概要：非同期読み込み付きのゲームシーンクラスの基底となるクラス
//------------------------------------------------------------------------------
class AsyncLoadScene : public Scene{
public:
	AsyncLoadScene() : bLoadCompleted_(false){ bAsync_ = true; }	// コンストラクタ
	virtual ~AsyncLoadScene(){}		// デストラクタ

	virtual void Load() = 0;		// 非同期読み込み

	bool IsLoadCompleted(){ return bLoadCompleted_; }	// 非同期読み込みが完了したかどうか

private:
	friend class Sequence;

	std::atomic<bool> bLoadCompleted_;	// 非同期読み込み完了フラグ
};

#endif	// #ifndef INCLUDE_IDEA_ASYNCLOADSCENE_H