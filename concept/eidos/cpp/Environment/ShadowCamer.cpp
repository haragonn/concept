#include "../../h/Environment/ShadowCamera.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../h/3D/Object.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

void ShadowCamera::Init(float viewAngle, float aspect, float nearZ, float farZ)
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return; }

	viewAngle_ = viewAngle;
	aspect_ = aspect;
	nearZ_ = nearZ;
	farZ_ = farZ;

	XMFLOAT4X4 matProj;
	XMStoreFloat4x4(&matProj, XMMatrixIdentity());
	XMStoreFloat4x4(&matProj, XMMatrixOrthographicLH((float)gm.GetWidth() / 50, (float)gm.GetHeight() / 50, 10.0f, 100.0f));
	//XMStoreFloat4x4(&matProj, XMMatrixPerspectiveFovLH(viewAngle_, aspect_, nearZ_, (farZ_ > nearZ_) ? farZ_ : nearZ_ + 0.0001f));
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			proj_.r[i][j] = matProj.m[i][j];
		}
	}

	eye_ = Vector3D(2.0f, 1.0f, 0.0f);
	eye_ = eye_.Normalized() * 50.0f;
	focus_ = Vector3D(0.0f, 0.0f, 0.0f);
	upY_ = 1.0f;

	viewPort_.topLeftX = 0.0f;
	viewPort_.topLeftY = 0.0f;
	viewPort_.width = (float)gm.GetWidth();
	viewPort_.height = (float)gm.GetHeight();
	viewPort_.minDepth = 0.0f;
	viewPort_.maxDepth = 1.0f;

	UpdateViewMatrix();

	//Camera::Init(viewAngle, aspect, nearZ, farZ);
}

void ShadowCamera::DrawObject()
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return; }

	gm.GetContextPtr()->ClearDepthStencilView(gm.GetDepthStencilViewPtr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);

	ID3D11RenderTargetView* pathShadow = gm.GetRenderTargetViewPtr(2);

	gm.GetContextPtr()->OMSetRenderTargets(1, &pathShadow, gm.GetDepthStencilViewPtr());
	gm.Draw3D();

	bShadowDraow_ = true;

	for(auto it = begin(vecObjPtr_), itEnd = end(vecObjPtr_); it != itEnd; ++it){
			(*it)->Draw(this);
	}

	bShadowDraow_ = false;

	//gm.EndMask();

	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;

	gm.DrawShadow(1, viewPort);

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	ID3D11RenderTargetView* pathMain = gm.GetRenderTargetViewPtr(0);
	gm.GetContextPtr()->ClearDepthStencilView(gm.GetDepthStencilViewPtr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);
	gm.GetContextPtr()->OMSetRenderTargets(1, &pathMain, gm.GetDepthStencilViewPtr());
	gm.Draw3D();
}