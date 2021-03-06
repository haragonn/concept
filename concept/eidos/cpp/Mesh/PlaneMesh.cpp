#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../h/Mesh/PlaneMesh.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/Utility/eidosType.h"
#include "../../h/Environment/Camera.h"
#include "../../h/Environment/ShadowCamera.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../idea/h/Utility/ideaUtility.h"

using namespace DirectX;

PlaneMesh::PlaneMesh() :
	uNum_(1U),
	vNum_(1U),
	pVertexBuffer_(nullptr),
	pIndexBuffer_(nullptr),
	pScmr_(nullptr)
{
}

bool PlaneMesh::Create(float centerX, float centerZ, float widthX, float widthZ, unsigned int uNum, unsigned int vNum)
{
	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()
		|| pVertexBuffer_){ return false; }

	widthX_ = widthX;
	widthZ_ = widthZ;
	uNum_ = max(1, uNum);
	vNum_ = max(1, vNum);

	centerX = widthX_ * uNum_ * 0.5f + centerX;
	centerZ = widthZ_ * vNum_ * 0.5f + centerZ;


	HRESULT hr;
	// 頂点バッファ
	{
		std::vector<MeshVertexData> vertexList;
		vertexList.resize((uNum_ + 1) * (vNum_ + 1));
		int cnt = 0;
		for(unsigned int j = 0; j < vNum_ + 1; ++j){
			for(unsigned int i = 0; i < uNum_ + 1; ++i){
				vertexList[cnt].pos.x = i * widthX_ - centerX;
				vertexList[cnt].pos.y = 0.0f;
				vertexList[cnt].pos.z = j * widthZ_ - centerZ;
				vertexList[cnt].nor.x = 0.0f;
				vertexList[cnt].nor.y = 1.0f;
				vertexList[cnt].nor.z = 0.0f;
				vertexList[cnt].tex.x = i % 2 ? 0.0f : 1.0f;
				vertexList[cnt].tex.y = j % 2 ? 0.0f : 1.0f;
				++cnt;
			}
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.ByteWidth = sizeof(MeshVertexData) * (uNum_ + 1) * (vNum_ + 1);
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &vertexList[0];
		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pVertexBuffer_);
		if(FAILED(hr)){ return false; }

	}

	// インデックスバッファ
	{
		unsigned int faceNum = uNum_ * vNum_;
		std::vector<WORD> indexList;
		std::vector<WORD>().swap(indexList);

		unsigned int cnt = 0U;

		indexList.push_back(0 + cnt);
		indexList.push_back(uNum_ + 1 + cnt);
		indexList.push_back(1 + cnt);
		indexList.push_back(uNum_ + 2 + cnt);
		indexList.push_back(uNum_ + 2 + cnt);
		++cnt;
		while(cnt < faceNum + vNum_){
			indexList.push_back(0 + cnt);
			indexList.push_back(0 + cnt);
			indexList.push_back(uNum_ + 1 + cnt);
			indexList.push_back(1 + cnt);
			indexList.push_back(uNum_ + 2 + cnt);
			indexList.push_back(uNum_ + 2 + cnt);
			++cnt;
			if((cnt + 1) % (uNum_ + 1) == 0)
				++cnt;
		}
		if(faceNum != 1 && cnt < faceNum + vNum_){
			indexList.push_back(0 + cnt);
			indexList.push_back(0 + cnt);
			indexList.push_back(uNum_ + 1 + cnt);
			indexList.push_back(1 + cnt);
			indexList.push_back(uNum_ + 2 + cnt);
		}
		indexNum_ = indexList.size();

		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(WORD) * indexNum_;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &indexList[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;

		hr = gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pIndexBuffer_);
		if(FAILED(hr)){ return false; }
	}

	return true;
}

void PlaneMesh::Release()
{
	SafeRelease(pVertexBuffer_);
	SafeRelease(pIndexBuffer_);

	ExclusionTexture();
}

void PlaneMesh::SetTexture(Texture& tex)
{
	TextureHolder::SetTexture(&tex);
}

void PlaneMesh::ExclusionTexture()
{
	TextureHolder::ExclusionTexture();
}

void PlaneMesh::SetShadow(ShadowCamera& scmr)
{
	pScmr_ = &scmr;
}

void PlaneMesh::Draw(Camera * pCamera)
{
	if(pScmr_ && !pScmr_->GetShadowDrawFlag()){
		if(!pTex_){
			DrawPlainShadow(pCamera);
		} else{
			DrawTexturePlainShadow(pCamera, *pTex_);
		}
	} else{
		if(!pTex_){
			DrawPlain(pCamera);
		} else{
			DrawTexturePlain(pCamera, *pTex_);
		}
	}
}

inline void PlaneMesh::DrawPlain(Camera * pCamera, int blend)
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !pVertexBuffer_
		|| !om.GetVertexShederPtr()
		|| !pCamera){ return; }

	// 定数バッファ
	SetConstBuffer(pCamera);

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort(pCamera);

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed(indexNum_, 0, 0);
}

inline void PlaneMesh::DrawPlainShadow(Camera* pCamera, int blend)
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !pVertexBuffer_
		|| !om.GetVertexShederPtr()
		|| !pCamera){ return; }

	// 定数バッファ
	SetConstBufferShadow(pCamera);

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtr(1);

	gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort(pCamera);

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed(indexNum_, 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

inline void PlaneMesh::DrawTexturePlain(Camera * pCamera, const Texture & tex, int blend)
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !pVertexBuffer_
		|| !om.GetVertexShederPtr()
		|| !pCamera){ return; }

	// 定数バッファ
	SetConstBuffer(pCamera);

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// テクスチャ書き込み
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	}else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort(pCamera);

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed(indexNum_, 0, 0);
}

inline void PlaneMesh::DrawTexturePlainShadow(Camera* pCamera, const Texture& tex, int blend)
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !pVertexBuffer_
		|| !om.GetVertexShederPtr()
		|| !pCamera){ return; }

	// 定数バッファ
	SetConstBufferShadow(pCamera);

	// 頂点バッファのセット
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	// インデックスバッファのセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetShadowInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// ブレンディングのセット
	if(!blend){
		gm.SetBlendState(BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_SUBTRACT);
	}

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);

	// テクスチャ書き込み
	{
		ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
		}
	}
	{
		ID3D11ShaderResourceView* pTexView = gm.GetShaderResourceViewPtr(1);

		gm.GetContextPtr()->PSSetShaderResources(1, 1, &pTexView);
	}

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetShadowVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(tex.GetTextureViewPtr()){
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderTextureShadowPtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(om.GetPixelShaderShadowPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	SetViewPort(pCamera);

	//ポリゴン描画
	gm.GetContextPtr()->DrawIndexed(indexNum_, 0, 0);

	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);
	gm.GetContextPtr()->PSSetShaderResources(1, 1, pSRV);
}

inline void PlaneMesh::SetConstBuffer(Camera* pCamera)
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){ return; }

	ConstBuffer3D cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	// 定数バッファ内容更新
	gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// 定数バッファ
	UINT cb_slot = 1;
	ID3D11Buffer* cb[1] = { om.GetConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
}

inline void PlaneMesh::SetConstBufferShadow(Camera* pCamera)
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();
	if(!gm.GetContextPtr()
		|| !om.GetVertexShederPtr()
		|| !pCamera){ return; }

	//定数バッファ
	ConstBuffer3DShadow cbuff;
	XMFLOAT4X4 matWorld;

	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matWorld.m[i][j] = world_.r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

	XMFLOAT4X4 matView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matView.m[i][j] = pCamera->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

	XMFLOAT4X4 matProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matProj.m[i][j] = pCamera->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

	XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));
	XMStoreFloat4(&cbuff.light, om.GetLight());

	XMFLOAT4X4 matLightView;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightView.m[i][j] = pScmr_->GetViewMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightView, XMMatrixTranspose(XMLoadFloat4x4(&matLightView)));

	XMFLOAT4X4 matLightProj;
	for(int i = 4 - 1; i >= 0; --i){
		for(int j = 4 - 1; j >= 0; --j){
			matLightProj.m[i][j] = pScmr_->GetProjectionMatrix().r[i][j];
		}
	}
	XMStoreFloat4x4(&cbuff.lightProj, XMMatrixTranspose(XMLoadFloat4x4(&matLightProj)));

	// 定数バッファ内容更新
	gm.GetContextPtr()->UpdateSubresource(om.GetShadowConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// 定数バッファ
	UINT cb_slot = 3;
	ID3D11Buffer* cb[1] = { om.GetShadowConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
}

inline void PlaneMesh::SetViewPort(Camera* pCamera)
{
	// 準備ができていなければ終了
	GraphicManager& gm = GraphicManager::Instance();

	if(!gm.GetContextPtr()
		|| !pCamera){ return; }

	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;

	gm.GetContextPtr()->RSSetViewports(1, &viewPort);
}
