/*==============================================================================
	[BaseWindow.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Framework/BaseWindow.h"
#include <stdarg.h>
#include <vector>

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
BaseWindow::BaseWindow():
	hWnd_(0),
	msg_(0),
	wp_(0),
	lp_(0)
{
	ZeroMemory(&wcex_, sizeof(WNDCLASSEX));
	ZeroMemory(title_, sizeof(char) * 256);
}

//------------------------------------------------------------------------------
// ウィンドウタイトルの変更
// 引数　：書式文字列(const char* pFormat),可変長引数(...)
// 戻り値：成否
//------------------------------------------------------------------------------
bool BaseWindow::SetWindowFormatText(const char* pFormat, ...)
{
	va_list args;	// 可変個数の引数リスト

	// フォーマット指定された文字列を出力する
	va_start(args, pFormat);
	int len = _vscprintf(pFormat, args) + 1;
	std::vector<char> strBuffer;
	strBuffer.resize(len);
	vsprintf_s(&strBuffer[0], len, pFormat, args);
	va_end(args);

	strcpy_s(title_, 256, &strBuffer[0]);

	return !!SetWindowText(hWnd_, &strBuffer[0]);	// 文字列をタイトルに反映させる
}

//------------------------------------------------------------------------------
// メッセージの監視
// 引数　：なし
// 戻り値：アプリケーションを続行するか否か
//------------------------------------------------------------------------------
bool BaseWindow::ProcessMessage()
{
	MSG msg;	// メッセージ

	if(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)){	// メッセージは来ていないか
		if(!GetMessage(&msg, NULL, 0, 0)){ return false; }	// 終了のメッセージかきていればfalseを返す
		TranslateMessage(&msg);	// メッセージを返す
		DispatchMessage(&msg);
	}

	return true;
}

//------------------------------------------------------------------------------
// ウィンドウプロシージャ
// 引数　：ウィンドウのハンドル(HWND hWnd), メッセージの識別子(UINT msg)
// 　　　　最初のパラメータ(WPARAM wp)，2番目のパラメータ(LPARAM lp)
// 戻り値：メッセージ処理の結果
//------------------------------------------------------------------------------
LRESULT CALLBACK BaseWindow::WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{

#ifdef _WIN64
	BaseWindow* pThis = (BaseWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);	// ウィンドウに関連付けられているデータの取得
#else
	BaseWindow* pThis = (BaseWindow*)GetWindowLong(hWnd, GWL_USERDATA);	// ウィンドウに関連付けられているデータの取得
#endif
	if(!pThis){
		if(msg == WM_NCCREATE && lp){ pThis = (BaseWindow*)((LPCREATESTRUCT)lp)->lpCreateParams; }	// ウィンドウが作られた時

#ifdef _WIN64
		if(pThis){ SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pThis); }	// lpに格納されたCREATESTRUCT構造体へのポインターを渡す
#else
		if(pThis){ SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)pThis); }	// lpに格納されたCREATESTRUCT構造体へのポインターを渡す
#endif
	}

	if(pThis){
		// ウィンドウハンドルやメッセージ情報を格納する
		pThis->hWnd_ = hWnd;
		pThis->msg_ = msg;
		pThis->wp_ = wp;
		pThis->lp_ = lp;
		return pThis->MessageProc();
	}

	return DefWindowProc(hWnd, msg, wp, lp);
}

//------------------------------------------------------------------------------
// メッセージの管理
// 引数　：なし
// 戻り値：メッセージ処理の結果
//------------------------------------------------------------------------------
LRESULT BaseWindow::MessageProc()
{
	// メッセージ処理
	switch(msg_){
	case WM_CREATE:
		return OnCreate();
	case WM_CLOSE:
		return OnClose();
	case WM_DESTROY:
		return OnDestroy();
	case WM_ACTIVATE:
		return OnActivate();
	case WM_SETFOCUS:
		return OnSetFocus();
	case WM_KILLFOCUS:
		return OnKillFocus();
	case WM_MOVE:
		return OnMove();
	case WM_SIZE:
		return OnSize();
	case WM_PAINT:
		return OnPaint();
	case WM_KEYDOWN:
		return OnKeyDown();
	case WM_SYSKEYDOWN:
		return OnSysKeyDown();
	case WM_SETCURSOR:
		return OnSetCursor();
	}
	return DefaultProc();
}