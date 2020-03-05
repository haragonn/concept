/*==============================================================================
	[Framework.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../../Framework.h"
#include "../../h/Framework/Sequence.h"
#include "../../h/Framework/Scene.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/2D/Actor2DManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/2D/SpriteInstancingManager.h"
#include "../../../eidos/h/3D/ObjectManager.h"
#include "../../h/Input/InputManager.h"
#include "../../h/Sound/SoundManager.h"
#include "../../h/Network/NetworkManager.h"
#include "../../h/Storage/StorageManager.h"
#include "../../../eidos/h/eidosStorage/eidosStorageManager.h"
#include "../../h/Framework/FadeManager.h"
#include "../../h/Utility/Timer.h"

#include <stdio.h>
#include <exception>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//------------------------------------------------------------------------------
// 定数定義
//------------------------------------------------------------------------------
const DWORD WINDOW_STYLE = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);
const DWORD FULL_SCREEN_STYLE = WS_EX_TOPMOST | WS_VISIBLE | WS_POPUP;

//------------------------------------------------------------------------------
// メインループスレッド
//------------------------------------------------------------------------------
DWORD WINAPI MainLoopThread(void* vp)
{
	Framework* pfm = (Framework*)vp;

	pfm->MainLoop();

	ExitThread(0);
}

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Framework::Framework() :
	hInst_(nullptr),
	width_(640U),
	height_(480U),
	bWindowed_(true),
	bReady_(false),
	pInitScene_(nullptr),
	bExit_(false),
	bOnWindowed_(true),
	bOnFullscreen_(false),
	bChangeDispRequest_(false),
	bChangeDispReady_(true),
	targetFps_(60U),
	fps_(60U),
	bFpsShow_(false),
	frameSkipMax_(0),
	frameSkipCount_(0),
	bStorage_(false),
	hMainLoopThread_(nullptr){}

//------------------------------------------------------------------------------
// 初期化
// 引数　：インスタンス(HINSTANCE hInst),表示状態(int nCmdShow),幅(UINT width),
// 　　　　高さ(UINT height),ウィンドウモード(bool bWindowed),
// 　　　　ネットワーク許可(bool bNetwork),FPS(UINT fps),
// 　　　　クラスネーム(const char* pClassName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Framework::Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed, UINT fps, UINT frameSkipMax, bool bNetwork, const char* pClassName)
{
	if(bReady_){ return false; }

	// 引数を反映
	hInst_ = hInst;
	width_ = width;
	height_ = height;
	bWindowed_ = true;
	targetFps_ = fps;
	fps_ = (float)fps;
	frameSkipMax_ = frameSkipMax;
	frameSkipCount_ = 0;

	//========================================
	// ウィンドウ
	//========================================
	// ウィンドウクラスの登録
	wcex_.cbSize = sizeof(WNDCLASSEX);						// 構造体サイズ
	wcex_.style = CS_HREDRAW | CS_VREDRAW;					// ウィンドウクラススタイル
	wcex_.lpfnWndProc = (WNDPROC)WndProc;					// ウィンドウプロシージャ
	wcex_.cbClsExtra = 0;									// 補助領域サイズ
	wcex_.cbWndExtra = 0;									// 補助領域サイズ
	wcex_.hInstance = hInst_;								// インスタンスハンドル
	wcex_.hIcon = LoadIcon(NULL, IDI_APPLICATION);			// アイコン
	wcex_.hCursor = LoadCursor(NULL, IDC_ARROW);			// マウスカーソル
	wcex_.hbrBackground = (HBRUSH)(COLOR_WINDOWTEXT + 1);	// クライアント領域背景色
	wcex_.lpszMenuName = NULL;								// ウィンドウメニュー
	wcex_.lpszClassName = pClassName;						// ウィンドウクラス名
	wcex_.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		// 小アイコン
	if(!RegisterClassEx(&wcex_)){ return false; }

	// ウィンドウの作成
	LONG tmpWidth = (LONG)width_;
	LONG tmpHeight = (LONG)height_;
	RECT cr = { 0, 0, tmpWidth, tmpHeight };
	AdjustWindowRect(&cr, WINDOW_STYLE, FALSE);
	int ww = cr.right - cr.left;
	int wh = cr.bottom - cr.top;
	RECT dr;
	GetWindowRect(GetDesktopWindow(), &dr);
	int dw = dr.right - dr.left;
	int dh = dr.bottom - dr.top;
	int wx = (ww > dw)?0:(dw - ww) >> 1;
	int wy = (wh > dh)?0:(dh - wh) >> 1;
	hWnd_ = CreateWindow(wcex_.lpszClassName, pClassName, WINDOW_STYLE, wx, wy, ww, wh, NULL, NULL, hInst_, this);
	if(!hWnd_){ return false; }

	//========================================
	// DirectX
	//========================================
	if(!GraphicManager::Instance().Init(hWnd_, width_, height_, bWindowed_, targetFps_)){
		MessageBox(hWnd_, "DirectX11を初期化できませんでした", NULL, NULL);
		return false;
	}
	if(!SpriteManager::Instance().Init()){
		MessageBox(hWnd_, "2Dシェーダーを初期化できませんでした", NULL, NULL);
		return false;
	}
	if(!SpriteInstancingManager::Instance().Init()){
		MessageBox(hWnd_, "2Dシェーダーを初期化できませんでした(2)", NULL, NULL);
		return false;
	}
	if(!ObjectManager::Instance().Init()){
		MessageBox(hWnd_, "3Dシェーダーを初期化できませんでした", NULL, NULL);
		return false;
	}
	if(!InputManager::Instance().Init(hInst_, hWnd_)){
		MessageBox(hWnd_, "DirectInputを初期化できませんでした", NULL, NULL);
		return false;
	}
	if(!SoundManager::Instance().Init()){
		MessageBox(hWnd_, "DirectSoundを初期化できませんでした", NULL, NULL);
		return false;
	}

	//========================================
	// Winsock
	//========================================
	if(bNetwork){
		if(!NetworkManager::Instance().Init()){
			MessageBox(hWnd_, "Winsockを初期化できませんでした", NULL, NULL);
			return false;
		}
	}

	// フェードの初期化
	FadeManager::Instance().Init((float)width_, (float)height_);

	// ウィンドウの表示
	if(!bWindowed){
		bChangeDispRequest_ = true;
	}
	ValidateRect(hWnd_, 0);			// WM_PAINTが呼ばれないようにする
	UpdateWindow(hWnd_);			// クライアント領域の更新
	ShowWindow(hWnd_, nCmdShow);	// 表示状態の設定


	bStorage_ = true;				// ストレージの使用可能フラグをオンに
	bReady_ = true;					// 準備フラグをオンに

	return true;
}

//------------------------------------------------------------------------------
// 終了処理
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Framework::UnInit()
{
	if(!bReady_){ return; }

	bReady_ = false;	// 準備フラグをオフに

	if(hMainLoopThread_){
		// メインループスレッドの終了を待つ
#ifdef _DEBUG
		// デバッグなら3分たったらで殺しちゃえ
		if(WaitForSingleObject(hMainLoopThread_, 3 * 60 * 1000) == WAIT_TIMEOUT){
			MessageBox(hWnd_, "MainLoopThreadが終了しません。強制終了します。", NULL, NULL);
			TerminateThread(hMainLoopThread_, FALSE);
		}
#else
		WaitForSingleObject(hMainLoopThread_, INFINITE);
#endif

		CloseHandle(hMainLoopThread_);
		hMainLoopThread_ = nullptr;
	}

	// 各マネージャの終了処理
	eidosStorageManager::Instance().UnLoadAll();
	ObjectManager::Instance().UnInit();
	StorageManager::Instance().UnLoadAll();
	SpriteManager::Instance().UnInit();
	SpriteInstancingManager::Instance().UnInit();
	GraphicManager::Instance().UnInit();
	SoundManager::Instance().UnInit();
	InputManager::Instance().UnInit();
	NetworkManager::Instance().UnInit();
}

//------------------------------------------------------------------------------
// メインループ
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Framework::MainLoop()
{
	Sequence& sq = Sequence::Instance();				// シーケンスのインスタンス
	GraphicManager& gm = GraphicManager::Instance();	// グラフィックマネージャのインスタンス
	FadeManager& fm = FadeManager::Instance();			// フェードのインスタンス

	if(!sq.Init(pInitScene_)){	// シーケンスの初期化
		sq.UnInit();
		return;
	}

	Timer tmr;				// タイマー
	Timer tmrFps;			// FPS計測用タイマー
	double frameTime = 1000.0 / targetFps_;	// 1フレームの単位時間
	unsigned long long frameCount = 0U;		// フレーム数
	UINT fpsCount = 0U;		// FPSカウント

	tmr.SetStartTime();		// タイマー開始
	tmrFps.SetStartTime();	//

	while(bReady_){
		if(bChangeDispRequest_){	// ディスプレイモード切り替えリクエスト受け取り
			bChangeDispRequest_ = false;
			bChangeDispReady_ = false;
		}

		if(!bChangeDispReady_){		// ディスプレイモードを切り替えている最中なら中止
			frameCount = 1U;
			fpsCount = 0U;
			frameSkipCount_ = 0U;
			tmr.SetStartTime();
			tmrFps.SetStartTime();

			continue;
		}

		// FPSの計測
		if(fpsCount == targetFps_){
			fps_ = (float)(targetFps_ * 1000.0 / tmrFps.GetHighPrecisionElapsed());
			fpsCount = 0U;
			tmrFps.SetStartTime();
		}

		if(tmr.GetHighPrecisionElapsed() > frameTime* (frameCount + 1) || NetworkManager::Instance().GetSkipFlag()){	// 目標の時間がたったら
			// 入力
			InputManager::Instance().UpdateAll();

			// 更新
			int sqTemp = sq.Update(bReady_);
			if(sqTemp == -1){ break; }
			fm.Update();
			Actor2DManager::Instance().WriteDownPosition();

			// 描画
			if((frameSkipMax_ <= frameSkipCount_ || !NetworkManager::Instance().GetSkipFlag())){
				if(gm.BeginScene()){
					gm.EndMask();	// マスクのクリーン
					sq.Draw();		// シーケンスの描画
					fm.Draw();		// フェードの描画
					gm.EndScene();
					frameSkipCount_ = 0U;
				}
			}
			else{
				++frameSkipCount_;
			}

			// シーンの切り替え時フレームカウントを初期化する
			if(sqTemp == 1){
				frameCount = 0U;
				fpsCount = 0U;
				frameSkipCount_ = 0U;
				tmr.SetStartTime();
				tmrFps.SetStartTime();
			}

			// 回したフレームのカウント
			++frameCount;
			++fpsCount;
		}
		else{
			NetworkManager::Instance().WritingEnd();
			Sleep(1);	// 目標の時間が立っていなければ休む
		}

		if(NetworkManager::Instance().GetSkipFlag()){
			NetworkManager::Instance().WritingEnd();
			Sleep(1);
		}
	}

	sq.UnInit();
}

//------------------------------------------------------------------------------
// シーンの実行
// 引数　：最初のシーンのポインタ(Scene* pInitScene)
// 戻り値：なし
//------------------------------------------------------------------------------
void Framework::Run(Scene* pInitScene)
{
	if(!bReady_ || !pInitScene){ return; }	// 準備はいいか,シーンのNULLチェック

	if(bExit_){	// Run()の再呼び出しの処理
		SafeDelete(pInitScene);
		return;
	}

	pInitScene_ = pInitScene;

	DWORD ExitCode;

	// メインループは別スレッドに投げる
	hMainLoopThread_ = CreateThread(NULL, 0, MainLoopThread, this, 0, NULL);

	// メインスレッドではメッセージを処理,メインループスレッド,デバイスも監視する
	while(ProcessMessage()){
		if(!bChangeDispReady_){ ChangeDisplayMode(); }

		// 簡易FPS表示
		if(bFpsShow_){
			char temp[16] = {};
			sprintf_s(temp, " fps[%2.1f]", fps_);
			char buf[300] = {};
			strcpy_s(buf, 300, title_);
			strcat_s(buf, 300, temp);
			SetWindowText(hWnd_, buf);
		}

		// メインループスレッドとウィンドウの監視
		if(hMainLoopThread_){
			GetExitCodeThread(hMainLoopThread_, &ExitCode);
			if(ExitCode != STILL_ACTIVE){
				CloseHandle(hMainLoopThread_);
				hMainLoopThread_ = nullptr;
				break;
			}else{ Sleep(30); }		
		}
		else{ break; }
	}

	bExit_ = true;

	UnInit();
}

//------------------------------------------------------------------------------
// マウスカーソルの可視化の設定
// 引数　：ウィンドウモード時のマウスカーソルの表示フラグ(bool bOnWindowed)
// 　　　　フルスクリーンモード時のマウスカーソルの表示フラグ(bool bOnFullscreen)
// 戻り値：なし
//------------------------------------------------------------------------------
void Framework::SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen)
{
	// フラグの反映
	bOnWindowed_ = bOnWindowed;
	bOnFullscreen_ = bOnFullscreen;
}

//------------------------------------------------------------------------------
// ウィンドウモードの切り替え
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Framework::ChangeDisplayMode()
{
	bWindowed_ = !bWindowed_;	// ウィンドウモードフラグを切り替える

	GraphicManager::Instance().ChangeDisplayMode(bWindowed_);	// グラフィックマネージャにウィンドウモード切り替えを要請

	// ウィンドウの変更
	if(bWindowed_){	// ウインドウ時
		LONG width = (LONG)width_;
		LONG height = (LONG)height_;
		RECT cr = { 0, 0, width, height };
		AdjustWindowRect(&cr, WINDOW_STYLE, FALSE);

		int ww = cr.right - cr.left;
		int wh = cr.bottom - cr.top;
		RECT dr;
		GetWindowRect(GetDesktopWindow(), &dr);
		int dw = dr.right - dr.left;
		int dh = dr.bottom - dr.top;
		int wx = (ww > dw)?0:(dw - ww) >> 1;
		int wy = (wh > dh)?0:(dh - wh) >> 1;

		SetWindowLong(hWnd_, GWL_STYLE, WINDOW_STYLE);
		SetWindowPos(hWnd_, HWND_NOTOPMOST, wx, wy, ww, wh, SWP_SHOWWINDOW);
	}
	else{	// フルスクリーン時
		RECT dr;
		GetWindowRect(GetDesktopWindow(), &dr);

		int dw = dr.right - dr.left;
		int dh = dr.bottom - dr.top;

		SetWindowLong(hWnd_, GWL_STYLE, FULL_SCREEN_STYLE);
		SetWindowPos(hWnd_, HWND_TOPMOST, 0, 0, dw, dh, SWP_SHOWWINDOW);
	}

	// リクエスト解消
	bChangeDispRequest_ = false;
	bChangeDispReady_ = true;
}

//------------------------------------------------------------------------------
// ウィンドウが破棄される時
// 引数　：なし
// 戻り値：メッセージ処理の結果
//------------------------------------------------------------------------------
LRESULT Framework::OnDestroy()
{
	UnInit();

	PostQuitMessage(0);	// 終了メッセージを送る

	return 0;
}

//------------------------------------------------------------------------------
// ウィンドウが閉じられた時
// 引数　：なし
// 戻り値：メッセージ処理の結果
//------------------------------------------------------------------------------
LRESULT Framework::OnClose()
{
	DestroyWindow(hWnd_);

	return 0;
}

//------------------------------------------------------------------------------
// キーが入力された時
// 引数　：なし
// 戻り値：メッセージ処理の結果
//------------------------------------------------------------------------------
LRESULT Framework::OnKeyDown(){
	if(wp_ == VK_ESCAPE){	// エスケープキーが押されたら
		if(bWindowed_){	// ウィンドウ時
#ifdef _DEBUG
			// デバッグ時はいきなり終了
			SendMessage(hWnd_, WM_CLOSE, 0, 0);	// 終了
			return 0;
#endif
			// 終了確認を促す
			if(MessageBox(hWnd_, "ゲームを終了しますか？", "終了確認", MB_OKCANCEL) == IDOK){
				SendMessage(hWnd_, WM_CLOSE, 0, 0);	// 終了
			}
		}
		else{	// フルスクリーン時
			bChangeDispRequest_ = true;	// ウィンドウモードの切り替え
		}

		return 0;
	}
	else{
		return DefaultProc();
	}
}

//------------------------------------------------------------------------------
// システムキーが入力された時
// 引数　：なし
// 戻り値：メッセージ処理の結果
//------------------------------------------------------------------------------
LRESULT Framework::OnSysKeyDown()
{
	// Alt+エンターキーが押されたらウィンドウモードの切り替え
	if(wp_ != VK_RETURN){ return DefaultProc(); }

	if(!NetworkManager::Instance().IsConnect()){ bChangeDispRequest_ = true; }

	return 0;
}

//------------------------------------------------------------------------------
// ウィンドウ内でマウスカーソルが動いた時
// 引数　：なし
// 戻り値：メッセージ処理の結果
//------------------------------------------------------------------------------
LRESULT Framework::OnSetCursor()
{
	// フラグに合わせて表示を切り替える
	if(bWindowed_?!bOnWindowed_:!bOnFullscreen_){
		SetCursor(NULL);	// マウスを非表示

		return 0;
	}
	else{ return DefaultProc(); }
}