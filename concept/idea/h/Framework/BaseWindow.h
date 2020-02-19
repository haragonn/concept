/*==============================================================================
	[BaseWindow.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_BASEWINDOW_H
#define INCLUDE_IDEA_BASEWINDOW_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//------------------------------------------------------------------------------
// クラス名　：BaseWindow
// クラス概要：ウィンドウを扱うWindowアプリケーションの基底となるクラス
// 　　　　　　主にメッセージ管理を行う
//------------------------------------------------------------------------------
class BaseWindow{
public:
	BaseWindow();			// コンストラクタ
	virtual ~BaseWindow(){}	// デストラクタ
	bool SetWindowFormatText(const char* pFormat, ...);	// ウィンドウタイトルの変更

protected:
	HWND hWnd_;			// ウィンドウハンドル
	WNDCLASSEX wcex_;	// ウィンドウクラス
	UINT msg_;			// メッセージ
	WPARAM wp_;			// パラメータ
	LPARAM lp_;			// パラメータ
	char title_[256];	// ウインドウタイトル

	bool ProcessMessage();	// メッセージの監視

	// メッセージプロシージャ関連
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);	// ウィンドウプロシージャ
	virtual inline LRESULT MessageProc();										// メッセージの管理
	virtual LRESULT OnCreate()		{ return DefaultProc(); }					// ウィンドウが生成された時
	virtual LRESULT OnClose()		{ return DefaultProc(); }					// ウィンドウが閉じられた時
	virtual LRESULT OnDestroy()		{ PostQuitMessage(0); return 0; }			// ウィンドウが破棄される時
	virtual LRESULT OnActivate()	{ return DefaultProc(); }					// ウィンドウのアクティブ化,非アクティブ化の通知
	virtual LRESULT OnSetFocus()	{ return DefaultProc(); }					// キーボードフォーカスを得た時
	virtual LRESULT OnKillFocus()	{ return DefaultProc(); }					// キーボードフォーカスを失った時
	virtual LRESULT OnMove()		{ return DefaultProc(); }					// ウィンドウが移動された時
	virtual LRESULT OnSize()		{ return DefaultProc(); }					// ウィンドウのサイズが変更された時
	virtual LRESULT OnPaint()		{ return DefaultProc(); }					// 描画する時
	virtual LRESULT OnKeyDown()		{ return DefaultProc(); }					// キーが入力された時
	virtual LRESULT OnSysKeyDown()	{ return DefaultProc(); }					// システムキーが入力された時
	virtual LRESULT OnSetCursor()	{ return DefaultProc(); }					// ウィンドウ内でマウスカーソルが動いた時
	virtual LRESULT DefaultProc()	{ return DefWindowProc(hWnd_, msg_, wp_, lp_); }	// デフォルトの処理
};

#endif // #ifndef INCLUDE_IDEA_BASEWINDOW_H