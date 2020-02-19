/*==============================================================================
	[Timer.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TIMER_H
#define INCLUDE_IDEA_TIMER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mmsystem.h>

//------------------------------------------------------------------------------
// クラス名　：Timer
// クラス概要：時間計測を行うクラス
//------------------------------------------------------------------------------
class Timer{
public:
	Timer();	// コンストラクタ
	~Timer();	// デストラクタ
	void SetStartTime()	// 計測開始時刻の設定
	{
		dwStart_ = timeGetTime();
		if(bPerformanceCounter_){ QueryPerformanceCounter(&liStart_); }
	}
	unsigned long GetElapsed()	// 経過時間の取得
	{
		return static_cast<unsigned long>(timeGetTime() - dwStart_);
	}
	double GetHighPrecisionElapsed()	// 高精度な経過時間の取得
	{
		if(bPerformanceCounter_){
			QueryPerformanceCounter(&liNow_);
			return (liNow_.QuadPart - liStart_.QuadPart) * clock_;
		}
		return static_cast<double>(timeGetTime() - dwStart_);
	}

private:
	static bool s_bSetPeriod_;	// timeBeginPeriod()したフラグ
	DWORD dwStart_;				// 開始時刻
	LARGE_INTEGER liStart_, liNow_, liFreq_;	// 時刻
	double clock_;				// 1クロック単位
	bool bPerformanceCounter_;	// 高精度タイマーが存在するか
};

#endif // #ifndef INCLUDE_IDEA_TIMER_H