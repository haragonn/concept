/*==============================================================================
	[GraphicManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_GRAPHICMANAGER_H
#define INCLUDE_IDEA_GRAPHICMANAGER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>

// ブレンドステート
enum BLEND_STATE{
	BLEND_NONE = 0,
	BLEND_DEFAULT,		// アルファブレンドなし
	BLEND_ALIGNMENT,	// アルファブレンドあり
	BLEND_ADD,			// 加算合成
	BLEND_SUBTRACT,		// 減算合成
	BLEND_MULTIPLE,		// 乗算合成
	BLEND_MAX
};

//------------------------------------------------------------------------------
// クラス名　：GraphicManager
// クラス概要：シングルトン
// 　　　　　　デバイス,コンテキスト,デプスバッファ,ラスタライザの管理を行う
//------------------------------------------------------------------------------
class GraphicManager{
public:
	static GraphicManager& Instance()	// 唯一のインスタンスを返す
	{
		static GraphicManager s_Instance;
		return s_Instance;
	}

	bool Init(HWND hWnd, UINT width, UINT height, bool bWindowed, UINT fps = 60U, bool bMSAA = false);	// 初期化
	void UnInit();		// 終了処理

	bool BeginScene();	// 描画開始
	bool EndScene();	// 描画終了

	HWND GetHWND()const{ return hWnd_; }	// ウィンドウハンドルの取得
	int	GetWidth()const{ return width_; }	// スクリーン幅の取得
	int GetHeight()const{ return height_; }	// スクリーン高さの取得

	ID3D11Device* GetDevicePtr()const{ return pD3DDevice_; }								// Direct3Dデバイスの取得
	ID3D11DeviceContext* GetContextPtr()const{ return pImmediateContext_; }					// コンテキストの取得
	ID3D11DepthStencilView* GetDepthStencilViewPtr(){ return pDepthStencilView_; }			// デプスステンシルビューの取得
	ID3D11RenderTargetView* GetDefaultRenderTargetViewPtr(){ return pRenderTargetView_; }	// 標準レンダーターゲットビューの取得
	ID3D11RasterizerState* GetDefaultRasterizerStatePtr(){ return pRsState_; }				// 標準ラスタライザ―ステートの取得
	ID3D11DepthStencilState* GetDefaultDepthStatePtr(){ return pDsState_; }					// 標準深度ステートの取得

	bool SetBlendState(BLEND_STATE blendState);	// ブレンドステートを設定する

	bool DrawMask(bool bVisible);	// マスクの描画
	bool DrawAnd();					// マスクのAND判定
	bool DrawXor();					// マスクのXOR判定
	bool EndMask();					// マスクの終了

	bool ChangeDisplayMode(bool bWindowed);		// ウィンドウモードの切り替え

private:
	HWND hWnd_;			// ウィンドウハンドル
	int width_;			// スクリーンの幅
	int height_;		// スクリーンの高さ
	bool bWindowed_;	// ウィンドウモード

	ID3D11Device* pD3DDevice_;					// デバイス
	BLEND_STATE blendState_;					// ブレンドステート
	ID3D11DeviceContext* pImmediateContext_;	// コンテキスト
	IDXGISwapChain* pSwapChain_;				// スワップチェイン
	D3D_DRIVER_TYPE driverType_;				// ドライバータイプ
	D3D_FEATURE_LEVEL featureLevel_;			// フューチャーレベル
	ID3D11DepthStencilView* pDepthStencilView_;	// デプスステンシルビュー
	ID3D11RenderTargetView* pRenderTargetView_;	// レンダーターゲットビュー
	ID3D11RasterizerState* pRsState_;			// ラスタライザ―ステート
	ID3D11DepthStencilState* pDsState_;			// デプスステンシルステート
	DXGI_SAMPLE_DESC MSAA_;

	UINT stencilRef_;				// ステンシルの値

	bool CreateRenderTarget();		// レンダーターゲットの作成

	GraphicManager();				// コンストラクタ
	~GraphicManager(){ UnInit(); }	// デストラクタ

	// コピーコンストラクタの禁止
	GraphicManager(const GraphicManager& src){}
	GraphicManager& operator=(const GraphicManager& src){}
};

#endif // #ifndef INCLUDE_IDEA_GRAPHICMANAGER_H