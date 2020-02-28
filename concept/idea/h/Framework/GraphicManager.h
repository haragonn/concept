/*==============================================================================
	[GraphicManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_GRAPHICMANAGER_H
#define INCLUDE_IDEA_GRAPHICMANAGER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../Utility/ideaMath.h"
#include "../Utility/ideaUtility.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>

// �u�����h�X�e�[�g
enum BLEND_STATE{
	BLEND_NONE = 0,
	BLEND_DEFAULT,		// �A���t�@�u�����h�Ȃ�
	BLEND_ALIGNMENT,	// �A���t�@�u�����h����
	BLEND_ADD,			// ���Z����
	BLEND_SUBTRACT,		// ���Z����
	BLEND_MULTIPLE,		// ��Z����
	BLEND_MAX
};

struct PeraVertex{
	Vector3D pos;
	Vector2D tex;
};

//------------------------------------------------------------------------------
// �N���X���@�FGraphicManager
// �N���X�T�v�F�V���O���g��
// �@�@�@�@�@�@�f�o�C�X,�R���e�L�X�g,�f�v�X�o�b�t�@,���X�^���C�U�̊Ǘ����s��
//------------------------------------------------------------------------------
class GraphicManager{
public:
	static const int RENDER_TARGET_VIEW_MAX = 4;

	static GraphicManager& Instance()	// �B��̃C���X�^���X��Ԃ�
	{
		static GraphicManager s_Instance;
		return s_Instance;
	}

	bool Init(HWND hWnd, UINT width, UINT height, bool bWindowed, UINT fps = 60U, bool bMSAA = false);	// ������
	void UnInit();		// �I������

	bool BeginScene();	// �`��J�n
	bool EndScene();	// �`��I��

	bool DrawPath(int target, int src, ID3D11PixelShader* pps, D3D11_VIEWPORT viewPort);	// �p�X�̕`��
	bool DrawShadow(int target, D3D11_VIEWPORT viewPort);	// �p�X�̕`��

	HWND GetHWND()const{ return hWnd_; }	// �E�B���h�E�n���h���̎擾
	int	GetWidth()const{ return width_; }	// �X�N���[�����̎擾
	int GetHeight()const{ return height_; }	// �X�N���[�������̎擾

	ID3D11Device* GetDevicePtr()const{ return pD3DDevice_; }								// Direct3D�f�o�C�X�̎擾

	ID3D11DeviceContext* GetContextPtr()const{ return pImmediateContext_; }					// �R���e�L�X�g�̎擾

	ID3D11DepthStencilView* GetDepthStencilViewPtr(){ return pDepthStencilView_; }			// �f�v�X�X�e���V���r���[�̎擾
	ID3D11ShaderResourceView* GetDepthShaderResourceViewPtr(){ return pDepthShaderResourceView_; }			// �f�v�X�X�e���V���r���[�̎擾
	ID3D11PixelShader* GetPeraShadowPixelShaderPtr(){
		return pPeraShadowPixelShader_;
	}

	ID3D11RenderTargetView* GetRenderTargetViewPtr(int idx)									// �����_�[�^�[�Q�b�g�r���[�̎擾
	{
		Assert(idx < RENDER_TARGET_VIEW_MAX);
		return pRenderTargetViews_[idx];
	}
	ID3D11Buffer* GetPeraVertexBufferPtr()const{ return pPeraVertexBuffer_; }				// �y���|���S���̒��_�o�b�t�@�|�C���^�̎擾

	ID3D11RasterizerState* GetDefaultRasterizerStatePtr(){ return pRsState_; }				// �W�����X�^���C�U�\�X�e�[�g�̎擾
	ID3D11DepthStencilState* GetDefaultDepthStatePtr(){ return pDsState_; }					// �W���[�x�X�e�[�g�̎擾

	bool SetBlendState(BLEND_STATE blendState);	// �u�����h�X�e�[�g��ݒ肷��

	bool DrawMask(bool bVisible);	// �}�X�N�̕`��
	bool DrawAnd();					// �}�X�N��AND����
	bool DrawXor();					// �}�X�N��XOR����
	bool EndMask();					// �}�X�N�̏I��
	bool Draw3D();

	bool ChangeDisplayMode(bool bWindowed);		// �E�B���h�E���[�h�̐؂�ւ�

private:
	HWND hWnd_;			// �E�B���h�E�n���h��
	int width_;			// �X�N���[���̕�
	int height_;		// �X�N���[���̍���
	bool bWindowed_;	// �E�B���h�E���[�h

	ID3D11Device* pD3DDevice_;					// �f�o�C�X

	BLEND_STATE blendState_;					// �u�����h�X�e�[�g

	ID3D11DeviceContext* pImmediateContext_;	// �R���e�L�X�g

	IDXGISwapChain* pSwapChain_;				// �X���b�v�`�F�C��

	D3D_DRIVER_TYPE driverType_;				// �h���C�o�[�^�C�v

	D3D_FEATURE_LEVEL featureLevel_;			// �t���[�`���[���x��

	ID3D11RenderTargetView* pRenderTargetViews_[RENDER_TARGET_VIEW_MAX];	// �����_�[�^�[�Q�b�g�r���[
	ID3D11ShaderResourceView* pShaderResourceViews_[RENDER_TARGET_VIEW_MAX];	// �V�F�[�_�[���\�[�X�r���[

	ID3D11DepthStencilView* pDepthStencilView_;	// �f�v�X�X�e���V���r���[
	ID3D11ShaderResourceView* pDepthShaderResourceView_;	// �V�F�[�_�[���\�[�X�r���[

	ID3D11RasterizerState* pRsState_;			// ���X�^���C�U�\�X�e�[�g
	ID3D11DepthStencilState* pDsState_;			// �f�v�X�X�e���V���X�e�[�g

	DXGI_SAMPLE_DESC MSAA_;						// �T���v���f�B�X�N

	ID3D11VertexShader* pPeraVertexShader_;

	ID3D11PixelShader* pDefaultPixelShader_;
	ID3D11PixelShader* pPeraPixelShader_;
	ID3D11PixelShader* pPeraShadowPixelShader_;

	ID3D11InputLayout* pPeraVertexLayout_;

	ID3D11Buffer* pPeraVertexBuffer_;

	UINT stencilRef_;				// �X�e���V���̒l

	bool CreateRenderTarget();		// �����_�[�^�[�Q�b�g�̍쐬

	GraphicManager();				// �R���X�g���N�^
	~GraphicManager(){ UnInit(); }	// �f�X�g���N�^

	// �R�s�[�R���X�g���N�^�̋֎~
	GraphicManager(const GraphicManager& src){}
	GraphicManager& operator=(const GraphicManager& src){}
};

#endif // #ifndef INCLUDE_IDEA_GRAPHICMANAGER_H