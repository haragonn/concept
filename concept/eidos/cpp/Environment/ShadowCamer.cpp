#include "../../h/Environment/ShadowCamera.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../h/3D/Object.h"

void ShadowCamera::Init(float viewAngle, float aspect, float nearZ, float farZ)
{
	Camera::Init(viewAngle, aspect, nearZ, farZ);
}

void ShadowCamera::DrawObject()
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return; }

	gm.GetContextPtr()->ClearDepthStencilView(gm.GetDepthStencilViewPtr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);

	ID3D11RenderTargetView* pathShadow = gm.GetRenderTargetViewPtr(3);

	gm.GetContextPtr()->OMSetRenderTargets(1, &pathShadow, gm.GetDepthStencilViewPtr());
	gm.Draw3D();

	for(auto it = begin(vecObjPtr_), itEnd = end(vecObjPtr_); it != itEnd; ++it){
			(*it)->Draw(this);
	}

	D3D11_VIEWPORT viewPort = {};
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (FLOAT)gm.GetWidth();
	viewPort.Height = (FLOAT)gm.GetHeight();
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(2, 1, pSRV);
	//gm.DrawShadow(1, viewPort);

	ID3D11RenderTargetView* pathMain = gm.GetRenderTargetViewPtr(0);
	gm.GetContextPtr()->ClearDepthStencilView(gm.GetDepthStencilViewPtr(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0x0);
	gm.GetContextPtr()->OMSetRenderTargets(1, &pathMain, gm.GetDepthStencilViewPtr());
	gm.Draw3D();
}