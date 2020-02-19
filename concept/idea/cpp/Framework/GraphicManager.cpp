/*==============================================================================
[GraphicManager.cpp]
Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Framework/GraphicManager.h"
#include "../../h/Utility/ideaUtility.h"
#include <vector>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
GraphicManager::GraphicManager() :
	hWnd_(NULL),
	width_(640),
	height_(480),
	bWindowed_(true),
	blendState_(BLEND_NONE),
	pD3DDevice_(nullptr),
	pImmediateContext_(nullptr),
	driverType_(D3D_DRIVER_TYPE_NULL),
	featureLevel_(D3D_FEATURE_LEVEL_11_0),
	pSwapChain_(nullptr),
	pDepthStencilView_(nullptr),
	pRenderTargetView_(nullptr),
	pRsState_(nullptr),
	pDsState_(nullptr),
	MSAA_({}),
	stencilRef_(0x0)
{}

//------------------------------------------------------------------------------
// 初期化
// 引数　：ウィンドウハンドル(HWND hWnd),幅(UINT width),高さ(UINT height)
// 　　　　ウィンドウモード(bool bWindowed)
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::Init(HWND hWnd, UINT width, UINT height, bool bWindowed, UINT fps, bool bMSAA)
{
	HRESULT hr;

	// 引数を反映
	hWnd_ = hWnd;
	width_ = width;
	height_ = height;
	bWindowed_ = bWindowed;

	UINT createDeviceFlags = 0U;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_DRIVER_TYPE driverTypes[]{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT driverTypesNum = ArraySize(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	UINT featureLevelsNum = ArraySize(featureLevels);

	// デバイス作成
	for(UINT i = 0; i < driverTypesNum; ++i) {
		hr = D3D11CreateDevice(NULL, driverTypes[i], NULL,
			createDeviceFlags, featureLevels, featureLevelsNum, D3D11_SDK_VERSION, &pD3DDevice_, &featureLevel_, &pImmediateContext_);
		if(SUCCEEDED(hr)){
			driverType_ = driverTypes[i];
			break;
		}
	}
	if(FAILED(hr)){ return false; }

	if(bMSAA){
		//使用可能なMSAAを取得
		ZeroMemory(&MSAA_, sizeof(MSAA_));
		for(int i = 0; i <= 4; ++i){	// Count = 4で十分
			UINT Quality = 0;
			if SUCCEEDED(pD3DDevice_->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality)){
				if(0 < Quality){
					MSAA_.Count = i;
					MSAA_.Quality = Quality - 1;
				}
			}
		}
	} else{
		MSAA_.Count = 1;
		MSAA_.Quality = 0;
	}

	// インターフェース取得
	IDXGIDevice1* pInterface = nullptr;
	hr = pD3DDevice_->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pInterface);
	if(FAILED(hr)){ return false; }

	// アダプター取得
	IDXGIAdapter* pAdapter = nullptr;
	hr = pInterface->GetAdapter(&pAdapter);
	if(FAILED(hr)){ return false; }
	SafeRelease(pInterface);	// もういらない

	// ファクトリー取得
	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);
	if(pFactory == nullptr){ return false; }
	SafeRelease(pAdapter);	// もういらない

	// スワップチェイン設定
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 4;
	sd.BufferDesc.Width = width_;
	sd.BufferDesc.Height = height_;
	sd.BufferDesc.RefreshRate.Numerator = fps;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd_;
	sd.SampleDesc = MSAA_;
	sd.Windowed = (bWindowed_) ? TRUE : FALSE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	hr = pFactory->CreateSwapChain(pD3DDevice_, &sd, &pSwapChain_);
	if(FAILED(hr)){ return false; }

	// ALT+Enterでのフルスクリーンは自前でやるのでオフに
	hr = pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
	if(FAILED(hr)){ return false; }
	SafeRelease(pFactory);	// もういらない

	// レンダーターゲットの設定
	if(!CreateRenderTarget()){ return false; }

	// ブレンディングステート生成
	if(!SetBlendState(BLEND_ALIGNMENT)){ return false; }

	// サンプラーステート生成
	ID3D11SamplerState* pSamplerState = nullptr;
	D3D11_SAMPLER_DESC samplerDesc = {};
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pD3DDevice_->CreateSamplerState(&samplerDesc, &pSamplerState);
	if(FAILED(hr)){ return false; }

	// サンプラーをコンテキストに設定
	pImmediateContext_->PSSetSamplers(0, 1, &pSamplerState);
	SafeRelease(pSamplerState);	// もういらない

	CD3D11_DEFAULT defaultState;

	//ラスタライザステート
	CD3D11_RASTERIZER_DESC rsdesc(defaultState);
	rsdesc.FillMode = D3D11_FILL_SOLID;
	rsdesc.CullMode = D3D11_CULL_NONE;
	hr = pD3DDevice_->CreateRasterizerState(&rsdesc, &pRsState_);
	if(FAILED(hr)) { return false; }
	pImmediateContext_->RSSetState(pRsState_);

	//デプスステンシルステート
	CD3D11_DEPTH_STENCIL_DESC dsdesc(defaultState);
	hr = pD3DDevice_->CreateDepthStencilState(&dsdesc, &pDsState_);
	if(FAILED(hr)) { return false; }
	pImmediateContext_->OMSetDepthStencilState(pDsState_, 0);

	// ビューポートの設定
	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)width_;
	viewPort.Height = (FLOAT)height_;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	pImmediateContext_->RSSetViewports(1, &viewPort);

	// 指定色で画面クリア
	static const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// 黒
	pImmediateContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);
	pImmediateContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);

	//ウインドウに反映
	pSwapChain_->Present(1, 0);
	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);

	return true;
}
//------------------------------------------------------------------------------
// 終了処理
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void GraphicManager::UnInit()
{
	// コンテキストを初期の状態に戻しておく
	if(pImmediateContext_){
		pImmediateContext_->ClearState();
	}

	// 解放
	SafeRelease(pDsState_);
	SafeRelease(pRsState_);
	SafeRelease(pDepthStencilView_);
	SafeRelease(pRenderTargetView_);
	SafeRelease(pSwapChain_);
	SafeRelease(pImmediateContext_);
	SafeRelease(pD3DDevice_);
}

//------------------------------------------------------------------------------
// 描画開始
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::BeginScene()
{
	// NULLチェック
	if(!pImmediateContext_){ return false; }

	// 指定色で画面クリア
	static const float clearColor[] = { 0.2f, 0.3f, 0.475f, 1.0f };	// 紺色
	pImmediateContext_->ClearRenderTargetView(pRenderTargetView_, clearColor);
	pImmediateContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);

	return true;
}

//------------------------------------------------------------------------------
// 描画終了
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::EndScene()
{
	// NULLチェック
	if(!pSwapChain_){ return false; }
	if(!pImmediateContext_){ return false; }

	//ウインドウに反映
	pSwapChain_->Present(1, 0);
	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);

	return true;
}

//------------------------------------------------------------------------------
// ブレンドステートを設定する
// 引数　：ブレンドステート(BLEND_STATE blendState)
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::SetBlendState(BLEND_STATE blendState)
{
	if(blendState == BLEND_NONE || blendState >= BLEND_MAX){ return false; }	// 無効なステータス
	if(blendState_ == blendState){ return true; }	// 変える必要がなければ返す

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;

	D3D11_RENDER_TARGET_BLEND_DESC renderTarget;

	switch(blendState){
	case BLEND_DEFAULT:	// アルファブレンドなし
		renderTarget.BlendEnable = FALSE;
		renderTarget.SrcBlend = D3D11_BLEND_ONE;
		renderTarget.DestBlend = D3D11_BLEND_ZERO;
		renderTarget.BlendOp = D3D11_BLEND_OP_ADD;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;

	case BLEND_ALIGNMENT:	// アルファブレンドあり
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		renderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		renderTarget.BlendOp = D3D11_BLEND_OP_ADD;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;

	case BLEND_ADD:			// 加算合成
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		renderTarget.DestBlend = D3D11_BLEND_ONE;
		renderTarget.BlendOp = D3D11_BLEND_OP_ADD;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;

	case BLEND_SUBTRACT:	// 減算合成
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		renderTarget.DestBlend = D3D11_BLEND_ONE;
		renderTarget.BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;

	case BLEND_MULTIPLE:	// 乗算合成
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D11_BLEND_ZERO;
		renderTarget.DestBlend = D3D11_BLEND_SRC_COLOR;
		renderTarget.BlendOp = D3D11_BLEND_OP_ADD;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;
	}

	CopyMemory(&blendDesc.RenderTarget[0], &renderTarget, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));

	ID3D11BlendState* pBlendState = nullptr;

	HRESULT hr = pD3DDevice_->CreateBlendState(&blendDesc, &pBlendState);
	if(FAILED(hr)){
		// 失敗したら後始末して返す
		if(pBlendState){
			pBlendState->Release();
			pBlendState = nullptr;
		}
		return false;
	}

	pImmediateContext_->OMSetBlendState(pBlendState, 0, 0xffffffff);

	if(pBlendState){	// もういらない
		pBlendState->Release();
		pBlendState = nullptr;
	}

	blendState_ = blendState;

	return true;
}

//------------------------------------------------------------------------------
// マスクの描画
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::DrawMask(bool bVisible)
{
	// 深度ステンシルステートを作成
	ID3D11DepthStencilState* pDepthStencilState = nullptr;
	D3D11_DEPTH_STENCIL_DESC ddsDesc;
	ZeroMemory(&ddsDesc, sizeof(ddsDesc));

	ddsDesc.DepthEnable = FALSE;
	ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ddsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	ddsDesc.StencilEnable = TRUE;
	ddsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	ddsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	ddsDesc.FrontFace.StencilFunc = bVisible ? D3D11_COMPARISON_EQUAL : D3D11_COMPARISON_NEVER;
	ddsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR_SAT;
	ddsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	ddsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_INCR_SAT;
	ddsDesc.BackFace.StencilFunc = bVisible ? D3D11_COMPARISON_EQUAL : D3D11_COMPARISON_NEVER;
	ddsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR_SAT;
	ddsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
	ddsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_INCR_SAT;

	if(FAILED(pD3DDevice_->CreateDepthStencilState(&ddsDesc, &pDepthStencilState))){ return false; }

	// 深度ステンシルをコンテキストに設定
	pImmediateContext_->OMSetDepthStencilState(pDepthStencilState, stencilRef_++);
	pDepthStencilState->Release();

	return true;
}

//------------------------------------------------------------------------------
// マスクのAND判定
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::DrawAnd()
{
	// 深度ステンシルステートを作成
	ID3D11DepthStencilState* pDepthStencilState = nullptr;
	D3D11_DEPTH_STENCIL_DESC ddsDesc;
	ZeroMemory(&ddsDesc, sizeof(ddsDesc));

	ddsDesc.DepthEnable = FALSE;
	ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ddsDesc.DepthFunc = D3D11_COMPARISON_EQUAL;
	ddsDesc.StencilEnable = TRUE;
	ddsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	ddsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	ddsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	ddsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	ddsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	if(FAILED(pD3DDevice_->CreateDepthStencilState(&ddsDesc, &pDepthStencilState))){ return false; }

	// 深度ステンシルをコンテキストに設定
	pImmediateContext_->OMSetDepthStencilState(pDepthStencilState, stencilRef_);
	pDepthStencilState->Release();

	return true;
}

//------------------------------------------------------------------------------
// マスクのXOR判定
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::DrawXor()
{
	// 深度ステンシルステートを作成
	ID3D11DepthStencilState* pDepthStencilState = nullptr;
	D3D11_DEPTH_STENCIL_DESC ddsDesc;
	ZeroMemory(&ddsDesc, sizeof(ddsDesc));

	ddsDesc.DepthEnable = FALSE;
	ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ddsDesc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
	ddsDesc.StencilEnable = TRUE;
	ddsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	ddsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	ddsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	ddsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
	ddsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	if(FAILED(pD3DDevice_->CreateDepthStencilState(&ddsDesc, &pDepthStencilState))){ return false; }

	// 深度ステンシルをコンテキストに設定
	pImmediateContext_->OMSetDepthStencilState(pDepthStencilState, stencilRef_);
	pDepthStencilState->Release();

	return true;
}

//------------------------------------------------------------------------------
// マスクの終了
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::EndMask()
{
	stencilRef_ = 0x0;

	// 深度ステンシルステートを作成
	ID3D11DepthStencilState* pDepthStencilState = nullptr;
	D3D11_DEPTH_STENCIL_DESC ddsDesc;
	ZeroMemory(&ddsDesc, sizeof(ddsDesc));

	ddsDesc.DepthEnable = FALSE;
	ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ddsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	ddsDesc.StencilEnable = TRUE;
	ddsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	ddsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	ddsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ddsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	ddsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	ddsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	ddsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ddsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	ddsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	ddsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;

	if(FAILED(pD3DDevice_->CreateDepthStencilState(&ddsDesc, &pDepthStencilState))){ return false; }

	// 深度ステンシルをコンテキストに設定
	pImmediateContext_->OMSetDepthStencilState(pDepthStencilState, 0x0);
	pDepthStencilState->Release();

	pImmediateContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0); // 深度初期化

	return true;
}

//------------------------------------------------------------------------------
// ウィンドウモードの切り替え
// 引数　：ウィンドウモード(bool bWindowed)
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::ChangeDisplayMode(bool bWindowed)
{
	if(!pSwapChain_){ return false; }
	if(bWindowed == bWindowed_){ return true; }	// 切り替える必要がなければ何もしない

	bWindowed_ = bWindowed;

	// バッファの解放
	ID3D11RenderTargetView*	tRTV = NULL;
	pImmediateContext_->OMSetRenderTargets(1, &tRTV, NULL);
	pDepthStencilView_->Release();
	pRenderTargetView_->Release();

	if(FAILED(pSwapChain_->ResizeBuffers(4, width_, height_, DXGI_FORMAT_R8G8B8A8_UNORM, 0))){ return false; }

	// レンダ―ターゲットの再設定
	if(!CreateRenderTarget()){ return false; }

	// ビューポートの設定
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)width_;
	viewPort.Height = (FLOAT)height_;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	pImmediateContext_->RSSetViewports(1, &viewPort);

	return true;
}

//------------------------------------------------------------------------------
// レンダーターゲットの作成
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool GraphicManager::CreateRenderTarget()
{
	// スワップチェインに用意されたバッファを取得
	ID3D11Texture2D* backBuffer = nullptr;
	HRESULT hr = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if(FAILED(hr)){ return false; }

	// レンダーターゲットの作成
	hr = pD3DDevice_->CreateRenderTargetView(backBuffer, NULL, &pRenderTargetView_);
	SafeRelease(backBuffer);
	if(FAILED(hr)){ return false; }

	// デプステクスチャの作成
	D3D11_TEXTURE2D_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(depthDesc));
	depthDesc.Width = width_;
	depthDesc.Height = height_;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc = MSAA_;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthBuffer = nullptr;
	hr = pD3DDevice_->CreateTexture2D(&depthDesc, NULL, &pDepthBuffer);
	if(FAILED(hr)){ return false; }

	// デプスステンシルビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	ZeroMemory(&dsvDesc, sizeof(dsvDesc));

	dsvDesc.Format = depthDesc.Format;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvDesc.Texture2D.MipSlice = 0;
	hr = pD3DDevice_->CreateDepthStencilView(pDepthBuffer, &dsvDesc, &pDepthStencilView_);
	if(pDepthBuffer){
		pDepthBuffer->Release();
		pDepthBuffer = nullptr;
	}
	if(FAILED(hr)){ return false; }

	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetView_, pDepthStencilView_);

	return true;
}
