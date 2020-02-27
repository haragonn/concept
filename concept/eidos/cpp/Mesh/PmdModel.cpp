#include "../../h/Mesh/PmdModel.h"
#include "../../h/eidosStorage/eidosStorageManager.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Archive/ArchiveLoader.h"
#include "../../../idea/h/Utility/ideaUtility.h"

#include <iostream>
#include <fstream>
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

PmdModel::PmdModel() :
	pVertexBuffer_(nullptr),
	pIndexBuffer_(nullptr),
	vertexSize_(0),
	indexSize_(0),
	materialSize_(0),
	boneSize_(0),
	texPtrSize_(0),
	bStorage_(false)
{
	vector<BlendVertexData>().swap(vecVertex_);
	vector<PmdMaterial>().swap(vecMaterial_);
	vector<PmdBone>().swap(vecPmdBone_);
	vector<unsigned short>().swap(vecIndex_);
	vector<Texture*>().swap(vecTexPtr_);
}


PmdModel::~PmdModel()
{
	UnLoad();
}

bool PmdModel::LoadPmdMeshFromFile(const char * pFileName)
{
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// 既に読み込み済みなら終了

	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return false; }

	ifstream ifs(pFileName, ios::binary);
	if(!ifs.is_open()){ return false; }

	//PMDヘッダー
	PmdHeader pmdHeader;
	ifs.read((char*)&pmdHeader, sizeof(pmdHeader));

	//頂点
	ifs.read((char*)&vertexSize_, sizeof(vertexSize_));

	vector<PmdVertex> vecTmpPV;
	vecTmpPV.resize(vertexSize_);
	ifs.read((char*)&vecTmpPV[0], sizeof(PmdVertex) * vertexSize_);

	vecVertex_.resize(vertexSize_);

	for(unsigned int i = 0; i < vertexSize_; ++i){
		vecVertex_[i].pos.x = vecTmpPV[i].pos.x;
		vecVertex_[i].pos.y = vecTmpPV[i].pos.y;
		vecVertex_[i].pos.z = vecTmpPV[i].pos.z;
		vecVertex_[i].nor.x = vecTmpPV[i].nor.x;
		vecVertex_[i].nor.y = vecTmpPV[i].nor.y;
		vecVertex_[i].nor.z = vecTmpPV[i].nor.z;
		vecVertex_[i].col = ideaColor::WHITE;
		vecVertex_[i].tex.x = vecTmpPV[i].tex.x;
		vecVertex_[i].tex.y = vecTmpPV[i].tex.y;
		vecVertex_[i].weight[0] = (float)(vecTmpPV[i].boneWeight) * 0.01f;
		vecVertex_[i].weight[1] = 1.0f - vecVertex_[i].weight[0];
		vecVertex_[i].weight[2] = 0.0f;
		vecVertex_[i].boneIndex[0] = vecTmpPV[i].boneNum[0];
		vecVertex_[i].boneIndex[1] = vecTmpPV[i].boneNum[1];
		vecVertex_[i].boneIndex[2] = 0;
		vecVertex_[i].boneIndex[3] = 0;
	}

	// 頂点インデックス
	ifs.read((char*)&indexSize_, sizeof(indexSize_));
	vecIndex_.resize(indexSize_);
	ifs.read((char*)&vecIndex_[0], sizeof(unsigned short) * indexSize_);

	//マテリアル
	ifs.read((char*)&materialSize_, sizeof(materialSize_));
	vecMaterial_.resize(materialSize_);
	ifs.read((char*)&vecMaterial_[0], sizeof(PmdMaterial) * materialSize_);

	for(unsigned int i = 0; i < materialSize_; ++i){
		Texture* pTex = new Texture;
		string t = vecMaterial_[i].textureFileName;
		string s = pFileName;
		AddDirectoryPath(t, s);
		pTex->LoadImageFromFile(t.c_str());
		vecTexPtr_.push_back(pTex);
	}
	texPtrSize_ = vecTexPtr_.size();

	//ボーン
	ifs.read((char*)&boneSize_, sizeof(boneSize_));
	vecPmdBone_.resize(boneSize_);
	ifs.read((char*)&vecPmdBone_[0], sizeof(PmdBone) * boneSize_);


	// ボーン情報格納
	vector<string> vecBoneName(boneSize_);

	for(unsigned int i = 0; i < boneSize_; ++i){
		auto& pd = vecPmdBone_[i];
		vecBoneName[i] = pd.boneName;
		auto& node = mapBone_[pd.boneName];
		node.index = i;
		node.pos = pd.boneHeadPos;
	}

	for(auto& pd : vecPmdBone_){
		if(pd.parentBoneIndex >= boneSize_ || pd.parentBoneIndex == 0xFFFF){
			continue;
		}
		auto parentName = vecBoneName[pd.parentBoneIndex];
		mapBone_[parentName].vecChildrenPtr.emplace_back(&mapBone_[pd.boneName]);
	}

	vecBoneMatrix_.resize(boneSize_);

	fill(vecBoneMatrix_.begin(), vecBoneMatrix_.end(), Matrix4x4::Identity());

	ifs.close();

	{
		// 頂点バッファを作成
		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(BlendVertexData) * vertexSize_;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &vecVertex_[0];
		if(FAILED(gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pVertexBuffer_))){ return false; }
	}

	{
		// インデックスバッファを作成
		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(unsigned short) * indexSize_;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &vecIndex_[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		if(FAILED(gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pIndexBuffer_))){ return false; }

	}

	return true;
}

bool PmdModel::LoadPmdMeshFromStorage(const char * pFileName)
{
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// 既に読み込み済みなら終了

	// ストレージから読み込む
	// 頂点
	vertexSize_ = eidosStorageManager::Instance().GetPmdModel(pFileName).vertexSize_;
	pVertexBuffer_ = eidosStorageManager::Instance().GetPmdModel(pFileName).pVertexBuffer_;
	copy(eidosStorageManager::Instance().GetPmdModel(pFileName).vecVertex_.begin(), eidosStorageManager::Instance().GetPmdModel(pFileName).vecVertex_.end(), back_inserter(vecVertex_));

	// インデックス
	indexSize_ = eidosStorageManager::Instance().GetPmdModel(pFileName).indexSize_;
	pIndexBuffer_ = eidosStorageManager::Instance().GetPmdModel(pFileName).pIndexBuffer_;
	copy(eidosStorageManager::Instance().GetPmdModel(pFileName).vecIndex_.begin(), eidosStorageManager::Instance().GetPmdModel(pFileName).vecIndex_.end(), back_inserter(vecIndex_));

	// ボーン
	boneSize_ = eidosStorageManager::Instance().GetPmdModel(pFileName).boneSize_;
	copy(eidosStorageManager::Instance().GetPmdModel(pFileName).vecPmdBone_.begin(), eidosStorageManager::Instance().GetPmdModel(pFileName).vecPmdBone_.end(), back_inserter(vecPmdBone_));

	// ボーン情報格納
	vector<string> vecBoneName(boneSize_);

	for(unsigned int i = 0; i < boneSize_; ++i){
		auto& pd = vecPmdBone_[i];
		vecBoneName[i] = pd.boneName;
		auto& node = mapBone_[pd.boneName];
		node.index = i;
		node.pos = pd.boneHeadPos;
	}

	for(auto& pd : vecPmdBone_){
		if(pd.parentBoneIndex >= boneSize_ || pd.parentBoneIndex == 0xFFFF){
			continue;
		}
		auto parentName = vecBoneName[pd.parentBoneIndex];
		mapBone_[parentName].vecChildrenPtr.emplace_back(&mapBone_[pd.boneName]);
	}

	vecBoneMatrix_.resize(boneSize_);

	fill(vecBoneMatrix_.begin(), vecBoneMatrix_.end(), Matrix4x4::Identity());

	// マテリアル
	materialSize_ = eidosStorageManager::Instance().GetPmdModel(pFileName).materialSize_;
	copy(eidosStorageManager::Instance().GetPmdModel(pFileName).vecMaterial_.begin(), eidosStorageManager::Instance().GetPmdModel(pFileName).vecMaterial_.end(), back_inserter(vecMaterial_));

	// テクスチャ
	texPtrSize_ = eidosStorageManager::Instance().GetPmdModel(pFileName).texPtrSize_;
	copy(eidosStorageManager::Instance().GetPmdModel(pFileName).vecTexPtr_.begin(), eidosStorageManager::Instance().GetPmdModel(pFileName).vecTexPtr_.end(), back_inserter(vecTexPtr_));

	if(pVertexBuffer_ && pIndexBuffer_){ bStorage_ = true; }	// ストレージ使用フラグをオンに

	return true;
}

void PmdModel::UnLoad()
{
	if(!bStorage_){
		SafeRelease(pIndexBuffer_);
		SafeRelease(pVertexBuffer_);
		for(auto it = vecTexPtr_.begin(); it != vecTexPtr_.end(); ++it){
			if(*it){
				(*it)->UnLoad();
			}
			SafeDelete((*it));
		}
	}
	bStorage_ = false;

	pVertexBuffer_ = nullptr;
	pIndexBuffer_ = nullptr;

	vertexSize_ = 0;
	vector<BlendVertexData>().swap(vecVertex_);

	indexSize_ = 0;
	vector<WORD>().swap(vecIndex_);

	boneSize_ = 0;
	vector<PmdBone>().swap(vecPmdBone_);

	map<std::string, Bone>().swap(mapBone_);

	vector<Matrix4x4>().swap(vecBoneMatrix_);

	materialSize_ = 0;
	vector<PmdMaterial>().swap(vecMaterial_);

	texPtrSize_ = 0;
	vector<Texture*>().swap(vecTexPtr_);
}

void PmdModel::Draw(Camera * pCamera)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	// 準備ができていなければ終了
	if(!gm.GetContextPtr()
		|| !om.GetVertexShederPtr()
		|| vertexSize_ <= 0
		|| !pVertexBuffer_
		|| !pIndexBuffer_
		|| !pCamera){
		return;
	}

	//定数バッファ
	{
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

		// 定数バッファをセット
		UINT cb_slot = 1;
		ID3D11Buffer* cb[1] = { om.GetConstBufferPtr() };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}

	//定数バッファ(pmd)
	{
		ConstBufferBoneWorld cbuff;
		ZeroMemory(&cbuff, sizeof(cbuff));

		vector<XMMATRIX> worlds;
		vector<XMMATRIX>().swap(worlds);
		worlds.resize(mapBone_.size());

		// ワールド行列の初期化
		XMMATRIX matIdentity = XMMatrixIdentity();

		for(unsigned int i = 0; i < 512; ++i){
			XMStoreFloat4x4(&cbuff.boneWorld[i], matIdentity);
		}

		for(unsigned int i = 0; i < vecBoneMatrix_.size() && i < 512; ++i){
			XMMATRIX mat;
			for(int j = 0; j < 4; ++j){
				for(int k = 0; k < 4; ++k){
					mat.r[j].m128_f32[k] = vecBoneMatrix_[i].r[j][k];
				}
			}

			XMStoreFloat4x4(&cbuff.boneWorld[i], mat);
		}

		// 定数バッファ内容更新
		gm.GetContextPtr()->UpdateSubresource(om.GetPmdConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

		// 定数バッファ
		UINT cb_slot = 2;
		ID3D11Buffer* cb[1] = { om.GetPmdConstBufferPtr() };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}

	//バーテックスバッファをセット
	ID3D11Buffer* pVBuf = pVertexBuffer_;
	UINT stride = sizeof(BlendVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	//インデックスバッファをセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetPmdInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetPmdVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// ビューポートの設定
	D3D11_VIEWPORT viewPort;
	viewPort.TopLeftX = pCamera->GetViewPort().topLeftX;
	viewPort.TopLeftY = pCamera->GetViewPort().topLeftY;
	viewPort.Width = pCamera->GetViewPort().width;
	viewPort.Height = pCamera->GetViewPort().height;
	viewPort.MinDepth = pCamera->GetViewPort().minDepth;
	viewPort.MaxDepth = pCamera->GetViewPort().maxDepth;
	gm.GetContextPtr()->RSSetViewports(1, &viewPort);

	// マテリアル毎に描画
	if(materialSize_ <= 1){
		ID3D11ShaderResourceView* pTexView = nullptr;
		if(texPtrSize_ > 0){
			pTexView = vecTexPtr_[0]->GetTextureViewPtr();
		}
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
		} else{
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
		}

		gm.GetContextPtr()->DrawIndexed(indexSize_, 0, 0);
	} else{
		unsigned long strat = 0U;
		for(unsigned int i = 0; i < materialSize_; ++i){
			ID3D11ShaderResourceView* pTexView = nullptr;
			if(texPtrSize_ > i){
				pTexView = vecTexPtr_[i]->GetTextureViewPtr();
			}
			if(pTexView){
				gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
			} else{
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
			}

			gm.GetContextPtr()->DrawIndexed(vecMaterial_[i].faceCount, strat, 0);

			strat += vecMaterial_[i].faceCount;
		}
	}
}