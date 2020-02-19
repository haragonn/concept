
#include "../../h/Mesh/FbxModel.h"
#include <iostream>
#include <fstream>
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../h/eidosStorage/eidosStorageManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Archive/ArchiveLoader.h"
#include "../../../idea/h/Utility/ideaUtility.h"
// ワーニングがうっとうしい
//#pragma warning(disable: 4100)
//#pragma warning(disable: 4512)

//#pragma comment(lib, "libfbxsdk-md.lib")
#pragma comment(lib, "libfbxsdk-mt.lib")
void AddDirectoryPath(std::string& fileName, std::string& src);
using namespace std;
using namespace DirectX;

FbxModel::FbxModel() :
	pFbxManager_(nullptr),
	pFbxScene_(nullptr),
	pFbxMesh_(nullptr),
	pVertexBuffer_(nullptr),
	pIndexBuffer_(nullptr),
	vertexSize_(0),
	indexSize_(0),
	subsetSize_(0),
	materialSize_(0),
	texPtrSize_(0),
	bStorage_(false)
{
	vector<MeshVertexData>().swap(vecVertex_);
	vector<ObjSubset>().swap(vecSubset_);
	vector<ObjMaterial>().swap(vecMaterial_);
	vector<WORD>().swap(vecIndex_);
	vector<Texture*>().swap(vecTexPtr_);
}

FbxModel::~FbxModel()
{
	UnLoad();
}

void FbxModel::SetupNode(FbxNode* pNode, std::string parentName, int faceStart, const char * pFileName)
{
	if(!pNode){ return; }

	FbxMesh* pMesh = pNode->GetMesh();

	if(pMesh)
	{
		int polygonCnt = pMesh->GetPolygonCount();
		for(int i = 0; i < polygonCnt; ++i)
		{
			int polygonSize = pMesh->GetPolygonSize(i);

			for(int j = 0; j < polygonSize; ++j)
			{
				int index = pMesh->GetPolygonVertex(i, j);
				vecIndex_.push_back(index);
				MeshVertexData vertex;
				ZeroMemory(&vertex, sizeof(vertex));
				vertex.pos.x = (float)pMesh->GetControlPointAt(index)[0];
				vertex.pos.y = (float)pMesh->GetControlPointAt(index)[2];
				vertex.pos.z = (float)pMesh->GetControlPointAt(index)[1];
				FbxVector4 nor;
				pMesh->GetPolygonVertexNormal(i, j, nor);
				vertex.nor.x = (float)nor.mData[0];
				vertex.nor.y = (float)nor.mData[2];
				vertex.nor.z = (float)nor.mData[1];
				FbxStringList	uvsetName;
				pMesh->GetUVSetNames(uvsetName);
				FbxString name = uvsetName.GetStringAt(0);
				FbxVector2 texCoord;
				bool unmapped = false;
				pMesh->GetPolygonVertexUV(i, j, name, texCoord, unmapped);
				vertex.tex.x = (float)texCoord.mData[0];
				vertex.tex.y = (float)abs(1.0f - texCoord.mData[1]);

				vecVertex_.push_back(vertex);
			}
		}
		//--- メッシュからノードを取得 ---//
		FbxNode* node = pNode;

		//--- マテリアルの数を取得 ---//
		int materialCount = node->GetMaterialCount();

		//--- マテリアルの数だけ繰り返す ---//
		for(int i = 0; materialCount > i; i++) {

			//--- マテリアルを取得 ---//
			FbxSurfaceMaterial* material = node->GetMaterial(i);
			FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

			//--- FbxLayeredTexture の数を取得 ---//
			int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

			//--- アタッチされたテクスチャが FbxLayeredTexture の場合 ---//
			if(0 < layeredTextureCount) {

				//--- アタッチされたテクスチャの数だけ繰り返す ---//
				for(int j = 0; layeredTextureCount > j; j++) {

					//--- テクスチャを取得 ---//
					FbxLayeredTexture* layeredTexture = prop.GetSrcObject<FbxLayeredTexture>(j);
					//--- レイヤー数を取得 ---//
					int textureCount = layeredTexture->GetSrcObjectCount<FbxFileTexture>();

					//--- レイヤー数だけ繰り返す ---//
					for(int k = 0; textureCount > k; k++) {
						//--- テクスチャを取得 ---//
						FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(k);

						if(texture) {
							//--- テクスチャ名を取得 ---//
							std::string textureName = texture->GetName();
							//std::string textureName = texture->GetRelativeFileName();

							//--- UVSet名を取得 ---//
							std::string UVSetName = texture->UVSet.Get().Buffer();

							Texture* pTex = new Texture;
							vecTexPtr_.push_back(pTex);
							string s = pFileName;
							AddDirectoryPath(textureName, s);
							vecTexPtr_[vecTexPtr_.size() - 1]->LoadImageFromFile(textureName.c_str());
						}
					}
				}
			} else {
				//--- テクスチャ数を取得 ---//
				int fileTextureCount = prop.GetSrcObjectCount<FbxFileTexture>();

				if(0 < fileTextureCount) {
					//--- テクスチャの数だけ繰り返す ---//
					for(int j = 0; fileTextureCount > j; j++) {
						//--- テクスチャを取得 ---//
						FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(j);
						if(texture) {
							//--- テクスチャ名を取得 ---//
							std::string textureName = texture->GetName();
							//std::string textureName = texture->GetRelativeFileName();

							//--- UVSet名を取得 ---//
							std::string UVSetName = texture->UVSet.Get().Buffer();

							Texture* pTex = new Texture;
							vecTexPtr_.push_back(pTex);
							string s = pFileName;
							AddDirectoryPath(textureName, s);
							vecTexPtr_[vecTexPtr_.size() - 1]->LoadImageFromFile(textureName.c_str());
						}
					}
				}
			}
		}
	}

	int childCnt = pNode->GetChildCount();
	for(int i = 0; i < childCnt; ++i)
	{
		SetupNode(pNode->GetChild(i), pNode->GetName(), faceStart, pFileName);
	}
}

bool FbxModel::LoadFbxMeshFromFile(const char * pFileName)
{
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// 既に読み込み済みなら終了

	GraphicManager& gm = GraphicManager::Instance();

	if(!gm.GetContextPtr()){ return false; }

	pFbxManager_ = FbxManager::Create();

	pFbxScene_ = FbxScene::Create(pFbxManager_, "Scene");

	FbxImporter *fbxImporter = FbxImporter::Create(pFbxManager_, "Importer");
	fbxImporter->Initialize(pFileName, -1, pFbxManager_->GetIOSettings());
	fbxImporter->Import(pFbxScene_);
	fbxImporter->Destroy();

	// 軸をDirectX用に
	FbxAxisSystem SceneAxisSystem = pFbxScene_->GetGlobalSettings().GetAxisSystem();
	if(SceneAxisSystem != FbxAxisSystem::DirectX)
	{
		FbxAxisSystem::DirectX.ConvertScene(pFbxScene_);
	}

	// 三角ポリゴン化
	FbxGeometryConverter geometryConverter(pFbxManager_);
	//geometryConverter.Triangulate(pFbxScene_, true);

	int meshCount = pFbxScene_->GetSrcObjectCount<FbxMesh>();

	//ObjSubset subset;
	//ZeroMemory(&subset, sizeof(subset));
	//subset.faceStart = vecIndex_.size();
	//vecSubset_.push_back(subset);

	SetupNode(pFbxScene_->GetRootNode(), "", 0, pFileName);

	// サイズの格納
	vertexSize_ = vecVertex_.size();
	indexSize_ = vecIndex_.size();
	subsetSize_ = vecSubset_.size();
	texPtrSize_ = vecTexPtr_.size();

	{
		// 頂点バッファを作成
		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(MeshVertexData) * vertexSize_;
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

	pFbxScene_->Destroy();
	pFbxManager_->Destroy();

	return true;
}

bool FbxModel::LoadFbxMeshFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// 既に読み込み済みなら終了

	GraphicManager& gm = GraphicManager::Instance();

	if(!gm.GetContextPtr()){ return false; }

	// tmpファイル名作成
	char tmpP[] = "p";
	char tempFileData[] = "fbx_";
	char tmpM[] = "m";
	char tempFileNum[] = "000";
	char tmpT[] = "t";
	char tempFileName[256] = "eidos_";
	char tmpDot[] = ".";
	strcat_s(tempFileName, tempFileData);
	strcat_s(tempFileName, tempFileNum);
	strcat_s(tempFileName, tmpDot);
	strcat_s(tempFileName, tmpT);
	strcat_s(tempFileName, tmpM);
	strcat_s(tempFileName, tmpP);

	// アーカイブファイルを開く
	ArchiveLoader loader;
	if(!loader.Load(pArchiveFileName, pFileName)){ return false; }

	// tmpファイル書き込み
	FILE* fp;

	if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

	if(fp == NULL) {
		return false;
	} else {
		fwrite(&loader.GetData()[0], loader.GetSize(), sizeof(BYTE), fp);
		fclose(fp);
	}

	// ファイルを開いて一時ストリームに書き込む
	ifstream file;
	file.open(tempFileName, ios::in);
	if(!file.is_open()){ return false; }

	stringstream tmpStream;
	tmpStream << file.rdbuf();

	file.close();

	// tmpファイルを念のため初期化
	if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

	if(fp == NULL) {
		return false;
	} else {
		fwrite("", 1, sizeof(BYTE), fp);
		fclose(fp);
	}

	DeleteFileA(tempFileName);	//tmpファイル削除


	return true;
}


void FbxModel::UnLoad()
{
	if(!bStorage_){
		SafeRelease(pIndexBuffer_);
		SafeRelease(pVertexBuffer_);
		for(auto it = vecTexPtr_.begin(); it != vecTexPtr_.end(); ++it){
			SafeDelete((*it));
		}
	}

	pVertexBuffer_ = nullptr;
	pIndexBuffer_ = nullptr;
	vertexSize_ = 0;
	indexSize_ = 0;
	subsetSize_ = 0;
	materialSize_ = 0;
	texPtrSize_ = 0;
	bStorage_ = false;
	vector<MeshVertexData>().swap(vecVertex_);
	vector<ObjSubset>().swap(vecSubset_);
	vector<ObjMaterial>().swap(vecMaterial_);
	vector<WORD>().swap(vecIndex_);
	vector<Texture*>().swap(vecTexPtr_);
}

void FbxModel::Draw(Camera * pCamera)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	// 準備ができていなければ終了
	if(!gm.GetContextPtr()
		|| !om.GetVertexShederPtr()
		|| !GetVertexBufferPtr()
		|| GetVertexSize() <= 0
		|| !GetIndexBufferPtr()
		|| !pCamera){
		return;
	}

	//定数バッファ
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

	//バーテックスバッファをセット
	ID3D11Buffer* pVBuf = GetVertexBufferPtr();
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);///

																		   //インデックスバッファをセット
	gm.GetContextPtr()->IASetIndexBuffer(GetIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);///

																							   // 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
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

	// サブセット毎に描画
	if(subsetSize_ <= 1){///
		ID3D11ShaderResourceView* pTexView = nullptr;
		if(texPtrSize_ > 0){///
			pTexView = vecTexPtr_[0]->GetTextureViewPtr();///
		}
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
		}
		if(pTexView){
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
		} else{
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
		}
		gm.GetContextPtr()->Draw(GetVertexSize(), 0);///
	} else{
		for(unsigned int i = 1; i < subsetSize_; ++i){///

			ID3D11ShaderResourceView* pTexView = nullptr;
			if(texPtrSize_ > i){///
				pTexView = vecTexPtr_[i - 1]->GetTextureViewPtr();///
			}
			if(pTexView){
				gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
			}
			if(pTexView){
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
			} else{
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
			}
			gm.GetContextPtr()->Draw(GetSubsetPtr()[i].faceStart - GetSubsetPtr()[i - 1].faceStart, GetSubsetPtr()[i - 1].faceStart);///
		}
		ID3D11ShaderResourceView* pTexView = nullptr;
		if(texPtrSize_ == subsetSize_ - 1){///
			pTexView = vecTexPtr_[subsetSize_ - 1]->GetTextureViewPtr();///
		}
		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
		}
		if(pTexView){
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
		} else{
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
		}
		gm.GetContextPtr()->Draw(GetVertexSize() - GetSubsetPtr()[subsetSize_ - 1].faceStart, GetSubsetPtr()[subsetSize_ - 1].faceStart);///
	}
}
