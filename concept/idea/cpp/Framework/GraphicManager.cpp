/*==============================================================================
[GraphicManager.cpp]
Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/Framework/GraphicManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/Utility/ideaMath.h"
#include "../../h/Utility/ideaUtility.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

//------------------------------------------------------------------------------
// �R���X�g���N�^
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
	pRenderTargetViews_(),
	pShaderResourceViews_(),
	pDepthStencilView_(nullptr),
	pDepthShaderResourceView_(nullptr),
	pRsState_(nullptr),
	pDsState_(nullptr),
	MSAA_({}),
	pPeraVertexShader_(nullptr),
	pDefaultPixelShader_(nullptr),
	pPeraPixelShader_(nullptr),
	pPeraShadowPixelShader_(nullptr),
	pPeraVertexLayout_(nullptr),
	pPeraVertexBuffer_(nullptr),
	stencilRef_(0x0){}

//------------------------------------------------------------------------------
// ������
// �����@�F�E�B���h�E�n���h��(HWND hWnd),��(UINT width),����(UINT height)
// �@�@�@�@�E�B���h�E���[�h(bool bWindowed)
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::Init(HWND hWnd, UINT width, UINT height, bool bWindowed, UINT fps, bool bMSAA)
{
	HRESULT hr = S_FALSE;

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
		//D3D_FEATURE_LEVEL_10_1,
		//D3D_FEATURE_LEVEL_10_0,
		//D3D_FEATURE_LEVEL_9_3,
		//D3D_FEATURE_LEVEL_9_2,
		//D3D_FEATURE_LEVEL_9_1,
	};
	UINT featureLevelsNum = ArraySize(featureLevels);

	// �f�o�C�X�쐬
	for(UINT i = 0; i < driverTypesNum; ++i) {
		hr = D3D11CreateDevice(NULL, driverTypes[i], NULL,
			createDeviceFlags, featureLevels, featureLevelsNum, D3D11_SDK_VERSION, &pD3DDevice_, &featureLevel_, &pImmediateContext_);
		if(SUCCEEDED(hr)){
			driverType_ = driverTypes[i];
			break;
		}
	}
	if(FAILED(hr)){ return false; }

	//if(bMSAA){
	//	//�g�p�\��MSAA���擾
	//	ZeroMemory(&MSAA_, sizeof(MSAA_));
	//	for(int i = 0; i <= 4; ++i){	// Count = 4�ŏ\��
	//		UINT Quality = 0;
	//		if SUCCEEDED(pD3DDevice_->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality)){
	//			if(0 < Quality){
	//				MSAA_.Count = i;
	//				MSAA_.Quality = Quality - 1;
	//			}
	//		}
	//	}
	//} else{
	//	MSAA_.Count = 1;
	//	MSAA_.Quality = 0;
	//}

	MSAA_.Count = 1;
	MSAA_.Quality = 0;

	// �C���^�[�t�F�[�X�擾
	IDXGIDevice1* pInterface = nullptr;
	hr = pD3DDevice_->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pInterface);
	if(FAILED(hr)){ return false; }

	// �A�_�v�^�[�擾
	IDXGIAdapter* pAdapter = nullptr;
	hr = pInterface->GetAdapter(&pAdapter);
	if(FAILED(hr)){
		SafeRelease(pInterface);
		return false;
	}

	SafeRelease(pInterface);	// ��������Ȃ�

	// �t�@�N�g���[�擾
	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);
	if(pFactory == nullptr){
		SafeRelease(pAdapter);
		return false;
	}

	SafeRelease(pAdapter);	// ��������Ȃ�

	// �X���b�v�`�F�C���ݒ�
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = width_;
	sd.BufferDesc.Height = height_;
	sd.BufferDesc.RefreshRate.Numerator = fps;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc = MSAA_;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 4;
	sd.OutputWindow = hWnd_;
	sd.Windowed = (bWindowed_) ? TRUE : FALSE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	hr = pFactory->CreateSwapChain(pD3DDevice_, &sd, &pSwapChain_);
	if(FAILED(hr)){
		SafeRelease(pFactory);
		return false;
	}

	// ALT+Enter�ł̃t���X�N���[���͎��O�ł��̂ŃI�t��
	hr = pFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER);
	if(FAILED(hr)){
		SafeRelease(pFactory);
		return false;
	}

	SafeRelease(pFactory);	// ��������Ȃ�

	// �����_�[�^�[�Q�b�g�̐ݒ�
	if(!CreateRenderTarget()){ return false; }

	// ���_�V�F�[�_�̓ǂݍ���
	{
		// ���_�V�F�[�_�쐬
		BYTE* data;
		int size = 0;
		size = ReadShader("VSPera.cso", &data);
		if(size == 0){ return false; }

		hr = pD3DDevice_->CreateVertexShader(data, size, NULL, &pPeraVertexShader_);
		if(FAILED(hr)){
			delete[] data;
			return false;
		}

		// ���̓��C�A�E�g��`
		D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		UINT elem_num = ARRAYSIZE(layout);

		// ���̓��C�A�E�g�쐬
		hr = pD3DDevice_->CreateInputLayout(layout, elem_num, data, size, &pPeraVertexLayout_);
		if(FAILED(hr)){
			delete[] data;
			return false;
		}

		delete[] data;
	}
	
	// �s�N�Z���V�F�[�_�̓ǂݍ���
	{
		BYTE* data;
		int size = 0;
		size = ReadShader("PSPeraDefault.cso", &data);
		if(size == 0){ return false; }

		hr = pD3DDevice_->CreatePixelShader(data, size, NULL, &pDefaultPixelShader_);
		if(FAILED(hr)){
			delete[] data;
			return false;
		}

		delete[] data;
	}
	{
		BYTE* data;
		int size = 0;
		size = ReadShader("PSPera.cso", &data);
		if(size == 0){ return false; }

		hr = pD3DDevice_->CreatePixelShader(data, size, NULL, &pPeraPixelShader_);
		if(FAILED(hr)){
			delete[] data;
			return false;
		}

		delete[] data;
	}
	{
		BYTE* data;
		int size = 0;
		size = ReadShader("PSPeraShadow.cso", &data);
		if(size == 0){ return false; }

		hr = pD3DDevice_->CreatePixelShader(data, size, NULL, &pPeraShadowPixelShader_);
		if(FAILED(hr)){
			delete[] data;
			return false;
		}

		delete[] data;
	}

	// ���_�o�b�t�@
	{
		PeraVertex pv[4] = {
			{{ -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }},
			{{ -1.0f,  1.0f, 0.0f }, { 0.0f, 0.0f }},
			{{  1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }},
			{{  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f }}};

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(PeraVertex) * 4;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA InitData = {};
		InitData.pSysMem = pv;

		hr = pD3DDevice_->CreateBuffer(&bd, &InitData, &pPeraVertexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	// �u�����f�B���O�X�e�[�g����
	if(!SetBlendState(BLEND_ALIGNMENT)){ return false; }

	{
		// �T���v���[�X�e�[�g����
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		ID3D11SamplerState* pSamplerState = nullptr;
		hr = pD3DDevice_->CreateSamplerState(&samplerDesc, &pSamplerState);
		if(FAILED(hr)){ return false; }

		// �T���v���[���R���e�L�X�g�ɐݒ�
		pImmediateContext_->PSSetSamplers(0, 1, &pSamplerState);
		SafeRelease(pSamplerState);	// ��������Ȃ�
	}

	CD3D11_DEFAULT defaultState;

	//���X�^���C�U�X�e�[�g
	CD3D11_RASTERIZER_DESC rsdesc(defaultState);
	rsdesc.FillMode = D3D11_FILL_SOLID;
	rsdesc.CullMode = D3D11_CULL_NONE;
	hr = pD3DDevice_->CreateRasterizerState(&rsdesc, &pRsState_);
	if(FAILED(hr)) { return false; }

	pImmediateContext_->RSSetState(pRsState_);

	//�f�v�X�X�e���V���X�e�[�g
	CD3D11_DEPTH_STENCIL_DESC dsdesc(defaultState);
	hr = pD3DDevice_->CreateDepthStencilState(&dsdesc, &pDsState_);
	if(FAILED(hr)) { return false; }

	pImmediateContext_->OMSetDepthStencilState(pDsState_, 0);

	//EndMask();

	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)width_;
	viewPort.Height = (FLOAT)height_;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	pImmediateContext_->RSSetViewports(1, &viewPort);

	// �o�b�N�o�b�t�@���Z�b�g
	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetViews_[0], pDepthStencilView_);

	// ��ʃN���A
	static const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };	// ��
	for(int i = RENDER_TARGET_VIEW_MAX - 1; i >= 0; --i){
		pImmediateContext_->ClearRenderTargetView(pRenderTargetViews_[i], clearColor);
	}
	pImmediateContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);

	//�E�C���h�E�ɔ��f
	pSwapChain_->Present(1, 0);

	return true;
}
//------------------------------------------------------------------------------
// �I������
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void GraphicManager::UnInit()
{
	// �R���e�L�X�g�������̏�Ԃɖ߂��Ă���
	if(pImmediateContext_){
		pImmediateContext_->ClearState();
	}

	// ���
	SafeRelease(pDsState_);
	SafeRelease(pRsState_);

	for(int i = RENDER_TARGET_VIEW_MAX - 1; i >= 0; --i){
		SafeRelease(pRenderTargetViews_[i]);
		SafeRelease(pShaderResourceViews_[i]);
	}

	SafeRelease(pDepthStencilView_);
	SafeRelease(pDepthShaderResourceView_);

	SafeRelease(pPeraVertexLayout_);

	SafeRelease(pPeraVertexBuffer_);

	SafeRelease(pPeraVertexShader_);

	SafeRelease(pDefaultPixelShader_);
	SafeRelease(pPeraPixelShader_);
	SafeRelease(pPeraShadowPixelShader_);

	SafeRelease(pSwapChain_);
	SafeRelease(pImmediateContext_);
	SafeRelease(pD3DDevice_);
}

//------------------------------------------------------------------------------
// �`��J�n
// �����@�F�Ȃ�
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::BeginScene()
{
	// NULL�`�F�b�N
	if(!pImmediateContext_){ return false; }

	pImmediateContext_->OMSetDepthStencilState(pDsState_, 0);

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	pImmediateContext_->PSSetShaderResources(0, 1, pSRV);
	pImmediateContext_->PSSetShaderResources(2, 1, pSRV);

	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetViews_[1], pDepthStencilView_);

	// ��ʃN���A
	static const float clearColor[] = { 0.2f, 0.3f, 0.475f, 1.0f };	// ���F
	for(int i = RENDER_TARGET_VIEW_MAX - 1; i >= 0; --i){
		pImmediateContext_->ClearRenderTargetView(pRenderTargetViews_[i], clearColor);
	}
	pImmediateContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);
	//EndMask();

	return true;
}

//------------------------------------------------------------------------------
// �`��I��
// �����@�F�Ȃ�
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::EndScene()
{
	// NULL�`�F�b�N
	if(!pSwapChain_){ return false; }
	if(!pImmediateContext_){ return false; }
	// �r���[�|�[�g�̐ݒ�
	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)width_;
	viewPort.Height = (FLOAT)height_;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	DrawShadow(0, viewPort);

	//DrawPath(0, 1, pDefaultPixelShader_, viewPort);

	viewPort.TopLeftX = width_ * -0.5f;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)width_;
	viewPort.Height = (FLOAT)height_;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	//DrawPath(0, 1, pPeraPixelShader_, viewPort);

	//�E�C���h�E�ɔ��f
	pSwapChain_->Present(1, 0);

	return true;
}

bool GraphicManager::DrawPath(int target, int src, ID3D11PixelShader* pps, D3D11_VIEWPORT viewPort)
{
	// NULL�`�F�b�N
	if(!pSwapChain_){ return false; }
	if(!pImmediateContext_){ return false; }

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	pImmediateContext_->PSSetShaderResources(0, 1, pSRV);

	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetViews_[target], nullptr);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(PeraVertex);
	UINT offset = 0;

	pImmediateContext_->IASetVertexBuffers(0, 1, &pPeraVertexBuffer_, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = pShaderResourceViews_[src];

	pImmediateContext_->PSSetShaderResources(0, 1, &pTexView);

	// ���̓��C�A�E�g�̃Z�b�g
	pImmediateContext_->IASetInputLayout(pPeraVertexLayout_);

	// �v���~�e�B�u�`��̃Z�b�g
	pImmediateContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �V�F�[�_�̃Z�b�g
	pImmediateContext_->VSSetShader(pPeraVertexShader_, NULL, 0);
	pImmediateContext_->HSSetShader(NULL, NULL, 0);
	pImmediateContext_->DSSetShader(NULL, NULL, 0);
	pImmediateContext_->GSSetShader(NULL, NULL, 0);
	pImmediateContext_->PSSetShader(pps, NULL, 0);
	pImmediateContext_->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	pImmediateContext_->RSSetViewports(1, &viewPort);

	//�|���S���`��
	pImmediateContext_->Draw(4, 0);

	return false;
}


bool GraphicManager::DrawShadow(int target, D3D11_VIEWPORT viewPort)
{
	// NULL�`�F�b�N
	if(!pSwapChain_){ return false; }
	if(!pImmediateContext_){ return false; }

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	pImmediateContext_->PSSetShaderResources(0, 1, pSRV);
	pImmediateContext_->PSSetShaderResources(2, 1, pSRV);

	pImmediateContext_->OMSetRenderTargets(1, &pRenderTargetViews_[target], nullptr);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(PeraVertex);
	UINT offset = 0;

	pImmediateContext_->IASetVertexBuffers(0, 1, &pPeraVertexBuffer_, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = pDepthShaderResourceView_;

	pImmediateContext_->PSSetShaderResources(2, 1, &pTexView);

	// ���̓��C�A�E�g�̃Z�b�g
	pImmediateContext_->IASetInputLayout(pPeraVertexLayout_);

	// �v���~�e�B�u�`��̃Z�b�g
	pImmediateContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �V�F�[�_�̃Z�b�g
	pImmediateContext_->VSSetShader(pPeraVertexShader_, NULL, 0);
	pImmediateContext_->HSSetShader(NULL, NULL, 0);
	pImmediateContext_->DSSetShader(NULL, NULL, 0);
	pImmediateContext_->GSSetShader(NULL, NULL, 0);
	pImmediateContext_->PSSetShader(pPeraShadowPixelShader_, NULL, 0);
	pImmediateContext_->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	pImmediateContext_->RSSetViewports(1, &viewPort);

	//�|���S���`��
	pImmediateContext_->Draw(4, 0);

	return false;
}

//------------------------------------------------------------------------------
// �u�����h�X�e�[�g��ݒ肷��
// �����@�F�u�����h�X�e�[�g(BLEND_STATE blendState)
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::SetBlendState(BLEND_STATE blendState)
{
	if(blendState == BLEND_NONE || blendState >= BLEND_MAX){ return false; }	// �����ȃX�e�[�^�X
	if(blendState_ == blendState){ return true; }	// �ς���K�v���Ȃ���ΕԂ�

	D3D11_BLEND_DESC blendDesc = {};

	D3D11_RENDER_TARGET_BLEND_DESC renderTarget = {};

	switch(blendState){
	case BLEND_DEFAULT:	// �A���t�@�u�����h�Ȃ�
		renderTarget.BlendEnable = FALSE;
		renderTarget.SrcBlend = D3D11_BLEND_ONE;
		renderTarget.DestBlend = D3D11_BLEND_ZERO;
		renderTarget.BlendOp = D3D11_BLEND_OP_ADD;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;

	case BLEND_ALIGNMENT:	// �A���t�@�u�����h����
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		renderTarget.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		renderTarget.BlendOp = D3D11_BLEND_OP_ADD;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;

	case BLEND_ADD:			// ���Z����
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		renderTarget.DestBlend = D3D11_BLEND_ONE;
		renderTarget.BlendOp = D3D11_BLEND_OP_ADD;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;

	case BLEND_SUBTRACT:	// ���Z����
		renderTarget.BlendEnable = TRUE;
		renderTarget.SrcBlend = D3D11_BLEND_SRC_ALPHA;
		renderTarget.DestBlend = D3D11_BLEND_ONE;
		renderTarget.BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
		renderTarget.SrcBlendAlpha = D3D11_BLEND_ONE;
		renderTarget.DestBlendAlpha = D3D11_BLEND_ZERO;
		renderTarget.BlendOpAlpha = D3D11_BLEND_OP_ADD;
		renderTarget.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		break;

	case BLEND_MULTIPLE:	// ��Z����
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
		SafeRelease(pBlendState);

		return false;
	}

	pImmediateContext_->OMSetBlendState(pBlendState, 0, 0xffffffff);

	SafeRelease(pBlendState);

	blendState_ = blendState;

	return true;
}

//------------------------------------------------------------------------------
// �}�X�N�̕`��
// �����@�F�Ȃ�
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::DrawMask(bool bVisible)
{
	// �[�x�X�e���V���X�e�[�g���쐬
	ID3D11DepthStencilState* pDepthStencilState = nullptr;

	CD3D11_DEFAULT defaultState;

	//�f�v�X�X�e���V���X�e�[�g
	CD3D11_DEPTH_STENCIL_DESC ddsDesc(defaultState);
	ddsDesc.DepthEnable = FALSE;
	ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
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

	// �[�x�X�e���V�����R���e�L�X�g�ɐݒ�
	pImmediateContext_->OMSetDepthStencilState(pDepthStencilState, stencilRef_++);

	SafeRelease(pDepthStencilState);

	return true;
}

//------------------------------------------------------------------------------
// �}�X�N��AND����
// �����@�F�Ȃ�
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::DrawAnd()
{
	// �[�x�X�e���V���X�e�[�g���쐬
	ID3D11DepthStencilState* pDepthStencilState = nullptr;

	CD3D11_DEFAULT defaultState;

	//�f�v�X�X�e���V���X�e�[�g
	CD3D11_DEPTH_STENCIL_DESC ddsDesc(defaultState);
	ddsDesc.DepthEnable = FALSE;
	ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
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

	// �[�x�X�e���V�����R���e�L�X�g�ɐݒ�
	pImmediateContext_->OMSetDepthStencilState(pDepthStencilState, stencilRef_);

	SafeRelease(pDepthStencilState);

	return true;
}

//------------------------------------------------------------------------------
// �}�X�N��XOR����
// �����@�F�Ȃ�
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::DrawXor()
{
	// �[�x�X�e���V���X�e�[�g���쐬
	ID3D11DepthStencilState* pDepthStencilState = nullptr;

	CD3D11_DEFAULT defaultState;

	//�f�v�X�X�e���V���X�e�[�g
	CD3D11_DEPTH_STENCIL_DESC ddsDesc(defaultState);
	ddsDesc.DepthEnable = FALSE;
	ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
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

	// �[�x�X�e���V�����R���e�L�X�g�ɐݒ�
	pImmediateContext_->OMSetDepthStencilState(pDepthStencilState, stencilRef_);

	SafeRelease(pDepthStencilState);

	return true;
}

//------------------------------------------------------------------------------
// �}�X�N�̏I��
// �����@�F�Ȃ�
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::EndMask()
{
	stencilRef_ = 0x0;

	// �[�x�X�e���V���X�e�[�g���쐬
	ID3D11DepthStencilState* pDepthStencilState = nullptr;

	CD3D11_DEFAULT defaultState;

	//�f�v�X�X�e���V���X�e�[�g
	CD3D11_DEPTH_STENCIL_DESC ddsDesc(defaultState);
	ddsDesc.DepthEnable = FALSE;
	ddsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	ddsDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	ddsDesc.StencilEnable = FALSE;
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

	// �[�x�X�e���V�����R���e�L�X�g�ɐݒ�
	pImmediateContext_->OMSetDepthStencilState(pDepthStencilState, 0x0);

	SafeRelease(pDepthStencilState);

	pImmediateContext_->ClearDepthStencilView(pDepthStencilView_, D3D11_CLEAR_STENCIL, 1.0f, 0x0);

	return true;
}

bool GraphicManager::Draw3D()
{
	pImmediateContext_->OMSetDepthStencilState(pDsState_, 0);

	return true;
}

//------------------------------------------------------------------------------
// �E�B���h�E���[�h�̐؂�ւ�
// �����@�F�E�B���h�E���[�h(bool bWindowed)
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::ChangeDisplayMode(bool bWindowed)
{
	if(!pSwapChain_){ return false; }

	if(bWindowed == bWindowed_){ return true; }	// �؂�ւ���K�v���Ȃ���Ή������Ȃ�

	bWindowed_ = bWindowed;

	// �r���[�̉��
	ID3D11RenderTargetView*	tRTV = NULL;
	pImmediateContext_->OMSetRenderTargets(1, &tRTV, NULL);

	SafeRelease(pDepthStencilView_);

	for(int i = RENDER_TARGET_VIEW_MAX - 1; i >= 0; --i){
		SafeRelease(pRenderTargetViews_[i]);
		SafeRelease(pShaderResourceViews_[i]);
	}

	if(FAILED(pSwapChain_->ResizeBuffers(4, width_, height_, DXGI_FORMAT_R8G8B8A8_UNORM, 0))){ return false; }

	// �����_�\�^�[�Q�b�g�̍Đݒ�
	if(!CreateRenderTarget()){ return false; }

	// �r���[�|�[�g�̐ݒ�
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
// �����_�[�^�[�Q�b�g�̍쐬
// �����@�F�Ȃ�
// �߂�l�F����
//------------------------------------------------------------------------------
bool GraphicManager::CreateRenderTarget()
{
	HRESULT hr = S_FALSE;

	// �o�b�N�o�b�t�@�����_�[�^�[�Q�b�g�̍쐬
	{
		// �X���b�v�`�F�C���ɗp�ӂ��ꂽ�o�b�t�@���擾
		ID3D11Texture2D* backBuffer = nullptr;
		hr = pSwapChain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		if(FAILED(hr)){ return false; }

		// �o�b�N�o�b�t�@�����_�[�^�[�Q�b�g�̍쐬
		hr = pD3DDevice_->CreateRenderTargetView(backBuffer, NULL, &pRenderTargetViews_[0]);
		if(FAILED(hr)){
			SafeRelease(backBuffer);
			return false;
		}

		SafeRelease(backBuffer);
	}

	// �����_�[�^�[�Q�b�g�̍쐬
	{
		D3D11_TEXTURE2D_DESC rtDesc = {};
		rtDesc.Width = width_;
		rtDesc.Height = height_;
		rtDesc.MipLevels = 1;
		rtDesc.ArraySize = 1;
		rtDesc.Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;
		rtDesc.SampleDesc = MSAA_;
		rtDesc.Usage = D3D11_USAGE_DEFAULT;
		rtDesc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = rtvDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		ID3D11Texture2D* renderTexes[RENDER_TARGET_VIEW_MAX] = {};

		for(int i = 1; i < RENDER_TARGET_VIEW_MAX; ++i) {
			hr = pD3DDevice_->CreateTexture2D(&rtDesc, 0, &renderTexes[i]);
			if(FAILED(hr)){
				SafeRelease(renderTexes[i]);			
				return false; 
			}

			hr = pD3DDevice_->CreateRenderTargetView(renderTexes[i], &rtvDesc, &pRenderTargetViews_[i]);
			if(FAILED(hr)){
				SafeRelease(renderTexes[i]);
				return false;
			}

			hr = pD3DDevice_->CreateShaderResourceView(renderTexes[i], &srvDesc, &pShaderResourceViews_[i]);
			if(FAILED(hr)){
				SafeRelease(renderTexes[i]);
				return false;
			}

			SafeRelease(renderTexes[i]);
		}
	}

	{
		// �f�v�X�e�N�X�`���̍쐬
		D3D11_TEXTURE2D_DESC depthDesc = {};
		depthDesc.Width = width_;
		depthDesc.Height = height_;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		depthDesc.SampleDesc = MSAA_;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

		ID3D11Texture2D* pDepthBuffer = nullptr;

		hr = pD3DDevice_->CreateTexture2D(&depthDesc, NULL, &pDepthBuffer);
		if(FAILED(hr)){
			SafeRelease(pDepthBuffer);
			return false;
		}

		// �f�v�X�X�e���V���r���[�̍쐬
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

		hr = pD3DDevice_->CreateDepthStencilView(pDepthBuffer, &dsvDesc, &pDepthStencilView_);
		if(FAILED(hr)){
			SafeRelease(pDepthBuffer);
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = pD3DDevice_->CreateShaderResourceView(pDepthBuffer, &srvDesc, &pDepthShaderResourceView_);
		if(FAILED(hr)){
			SafeRelease(pDepthBuffer);
			return false;
		}

		SafeRelease(pDepthBuffer);
	}

	pImmediateContext_->OMSetRenderTargets(ArraySize(pRenderTargetViews_), pRenderTargetViews_, pDepthStencilView_);

	return true;
}
