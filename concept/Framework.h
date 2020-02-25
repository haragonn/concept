/*==============================================================================
	[Framework.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FLAMEWORK_H
#define INCLUDE_IDEA_FLAMEWORK_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "idea/h/Framework/BaseWindow.h"
#include <atomic>

//------------------------------------------------------------------------------
// 前方宣言
//------------------------------------------------------------------------------
class Scene;

//------------------------------------------------------------------------------
// クラス名　：Framework
// クラス概要：シングルトン
// 　　　　　　ウィンドウの管理,ゲームのメインループを行う
//------------------------------------------------------------------------------
class Framework : public BaseWindow{
public:
	static Framework& Instance()	// 唯一のインスタンスを返す
	{
		static Framework s_Instance;
		return s_Instance;
	}
	bool Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed = true, UINT fps = 60U, UINT frameSkipMax = 0U, bool bMSAA = false, bool bNetwork = false, const char* pClassName = "idea");	// 初期化
	void UnInit();				// 終了処理
	void Run(Scene* pInitScene);	// シーンの実行
	float GetFps(){ return fps_; }				// FPSの取得
	UINT GetTargetFps(){ return targetFps_; }	// 目標FPSの取得
	void SetFpsSimpleDisplayFlag(bool bShow){ bFpsShow_ = bShow; }	// FPS簡易表示の設定
	void SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen);	// マウスカーソルの可視化の設定

private:
	HINSTANCE hInst_;			// インスタンスハンドル

	UINT width_;				// スクリーンの幅
	UINT height_;				// スクリーンの高さ
	bool bWindowed_;			// ウィンドウモード

	std::atomic<bool> bReady_;			// 実行準備
	std::atomic<Scene*> pInitScene_;	// 初期シーン

	bool bOnWindowed_;			// ウィンドウ時のマウスカーソル
	bool bOnFullscreen_;		// フルスクリーン時のマウスカーソル

	std::atomic<bool> bChangeDispRequest_;	// ウィンドウモードの切り替え要請
	std::atomic<bool> bChangeDispReady_;	// ウインドウモードの切り替えの完了

	std::atomic<UINT> targetFps_;		// 目標のFPS
	float fps_;							// 現在のFPS
	bool bFpsShow_;						// FPSの簡易表示フラグ
	std::atomic<UINT> frameSkipMax_;	// 最大フレームスキップ数
	std::atomic<UINT> frameSkipCount_;	// フレームスキップ回数

	bool bStorage_;				// ストレージの使用可能フラグ

	HANDLE hMainLoopThread_;	// メインループスレッドのハンドル

	friend DWORD WINAPI MainLoopThread(void* vp);	// メインループスレッド
	void MainLoop();			// メインループ
	void ChangeDisplayMode();	// ウィンドウモードの切り替え

	// メッセージ処理
	LRESULT OnDestroy();	// ウィンドウが破棄される時
	LRESULT OnClose();		// ウィンドウが閉じられた時
	LRESULT OnKeyDown();	// キーが入力された時
	LRESULT OnSysKeyDown();	// システムキーが入力された時
	LRESULT OnSetCursor();	// ウィンドウ内でマウスカーソルが動いた時

	Framework();			// コンストラクタ
	~Framework(){}			// デストラクタ

	// コピーコンストラクタの禁止
	Framework(Framework& src){}
	Framework& operator=(Framework& src){}
};

#endif // #ifndef INCLUDE_IDEA_FLAMEWORK_H