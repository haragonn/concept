/*==============================================================================
	[FadeManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FADEMANAGER_H
#define INCLUDE_IDEA_FADEMANAGER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../2D/Sprite.h"

//------------------------------------------------------------------------------
// クラス名　：FadeManager
// クラス概要：シングルトン
// 　　　　　　フェードの設定,描画を行う
//------------------------------------------------------------------------------
class FadeManager{
public:
	static FadeManager& Instance()	// 唯一のインスタンスを返す
	{
		static FadeManager s_Instance;
		return s_Instance;
	}

	void Init(float width, float height);	// 初期化
	void UnInit();	// 終了処理

	void Update();	// 更新

	void SetFade(int time, float r = 1.0f, float g = 1.0f, float b = 1.0f);	// フェードの設定
	bool IsFading()const{ return !!(int)(a_ + 0.999999f); }					// フェード中かどうか
	bool IsFadeOutCompleted()const{ return bFadeOutCompleted_; }			// フェードアウトが完了したかどうか

	void Draw();	// 描画

private:
	Sprite spr_;	// スプライト

	float r_;		// 赤成分
	float g_;		// 緑成分
	float b_;		// 青成分
	float a_;		// a値

	float value_;	// 変化量

	bool bFadeEnable_;			// 有効かどうか
	bool bFadeOut_;				// フェードアウトフラグ
	bool bFadeOutCompleted_;	// フェードアウト完了フラグ

	FadeManager() :	// コンストラクタ
		r_(1.0f), g_(1.0f), b_(1.0f), a_(0.0f), value_(1.0f),
		bFadeEnable_(false), bFadeOut_(false), bFadeOutCompleted_(false){}

	// コピーコンストラクタの禁止
	FadeManager(const FadeManager& src){}
	FadeManager& operator =(const FadeManager& src){}
};

#endif // #ifndef INCLUDE_IDEA_FADEMANAGER_H