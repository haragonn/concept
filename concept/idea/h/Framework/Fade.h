/*==============================================================================
	[Fade.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FADE_H
#define INCLUDE_IDEA_FADE_H

#include "../Utility/ideaColor.h"

//------------------------------------------------------------------------------
// クラス名　：Fade
// クラス概要：フェードの管理を行うクラス
//------------------------------------------------------------------------------
class Fade{
public:
	static void SetFade(int time, Color color);	// フェードの設定
	static void SetFade(int time, float r = 1.0f, float g = 1.0f, float b = 1.0f);	// フェードの設定
	static bool IsFading();						// フェードアウト中かどうか
	static bool IsFadeOutCompleted();				// フェードアウトが完了したかどうか
};

#endif	// #ifndef INCLUDE_IDEA_FADE_H