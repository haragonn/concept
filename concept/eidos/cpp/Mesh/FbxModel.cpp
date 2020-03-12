
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
	materialSize_(0),
	texPtrSize_(0),
	meshSize_(0),
	bStorage_(false)
{
	vector<MeshVertexData>().swap(vecVertex_);
	vector<FbxMaterial>().swap(vecMaterial_);
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
				vertex.color.r = 1.0f;
				vertex.color.g = 1.0f;
				vertex.color.b = 1.0f;
				vertex.color.a = 1.0f;
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
	bool hr =fbxImporter->Initialize(pFileName, -1, pFbxManager_->GetIOSettings());
	if(!hr){
		SafeDestroy(fbxImporter);
		pFbxScene_->Destroy();
		pFbxManager_->Destroy();
		return false;
	}
	fbxImporter->Import(pFbxScene_);
	SafeDestroy(fbxImporter);

	// 軸をDirectX用に
	FbxAxisSystem SceneAxisSystem = pFbxScene_->GetGlobalSettings().GetAxisSystem();
	if(SceneAxisSystem != FbxAxisSystem::DirectX)
	{
		FbxAxisSystem::DirectX.ConvertScene(pFbxScene_);
	}

	FbxGeometryConverter geometryConverter(pFbxManager_);

	// 三角ポリゴン化
	geometryConverter.Triangulate(pFbxScene_, true);

	// 縮退ポリゴンの削除
	geometryConverter.RemoveBadPolygonsFromMeshes(pFbxScene_);

	// マテリアルごとにメッシュを分離
	geometryConverter.SplitMeshesPerMaterial(pFbxScene_, true);

	//------------------------------------
	//FBXSDK読み込み
	//------------------------------------

	FbxMesh* mesh;
	meshSize_ = pFbxScene_->GetSrcObjectCount<FbxMesh>();
	int nNumMaterial = pFbxScene_->GetMaterialCount();

	vecMesh_.resize(meshSize_);
	vecMaterial_.resize(nNumMaterial);

	for(unsigned int i = 0; i < meshSize_; i++)
	{
		vecMesh_[i].pTexture = nullptr;
	}
	for(int i = 0; i < nNumMaterial; i++)
	{
		vecMaterial_[i].pTexture = nullptr;
	}
	//FbxArray<FbxString*> animation_names;
	//FbxTime start_time, end_time;
	//pFbxScene_->FillAnimStackNameArray(animation_names);
	//m_nNumAnime = animation_names.GetCount();
	//m_pAnime = new Anime[m_nNumAnime];


	//------------------------------------
	//マテリアル
	//------------------------------------
	for(int i = 0; i < nNumMaterial; i++)
	{
		FbxSurfaceLambert* lambert;
		FbxSurfacePhong* phong;

		FbxSurfaceMaterial* material = pFbxScene_->GetMaterial(i);
		// materialはKFbxSurfaceMaterialオブジェクト
		vecMaterial_[i].pMaterialName = (char*)material->GetName();

		// LambertかPhongか
		if(material->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			// Lambertにダウンキャスト
			lambert = (FbxSurfaceLambert*)material;

			FbxDouble3 diffuse = lambert->Diffuse;
			vecMaterial_[i].color.r = (float)diffuse[0];
			vecMaterial_[i].color.g = (float)diffuse[1];
			vecMaterial_[i].color.b = (float)diffuse[2];
			vecMaterial_[i].color.a = (float)diffuse[3];
		} else if(material->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			// Phongにダウンキャスト
			phong = (FbxSurfacePhong*)material;
		}


		// ディフューズプロパティを検索
		FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		vecMesh_[i].pTextureFileName = NULL;
		// プロパティが持っているレイヤードテクスチャの枚数をチェック
		int layerNum = property.GetSrcObjectCount<FbxLayeredTexture>();
		// レイヤードテクスチャが無ければ通常テクスチャ
		if(layerNum == 0)
		{
			// 通常テクスチャの枚数をチェック
			int numGeneralTexture = property.GetSrcObjectCount<FbxFileTexture>();
			numGeneralTexture = numGeneralTexture > 0 ? 1 : 0;///
			vecMesh_[i].pTextureFileName = NULL;
			// 各テクスチャについてテクスチャ情報をゲット
			for(int clusterCnt = 0; clusterCnt < numGeneralTexture; ++clusterCnt)
			{
				// i番目のテクスチャオブジェクト取得
				FbxFileTexture* texture = FbxCast<FbxFileTexture>(property.GetSrcObject<FbxFileTexture>(clusterCnt));

				// materialはKFbxSurfaceMaterialオブジェクト
				std::string textureName = texture->GetName();

				Texture* pTex = new Texture;
				vecTexPtr_.push_back(pTex);
				string s = pFileName;
				AddDirectoryPath(textureName, s);
				vecTexPtr_[vecTexPtr_.size() - 1]->LoadImageFromFile(textureName.c_str());

				vecMaterial_[i].pFileName = textureName.c_str();
				vecMaterial_[i].pTexture = pTex;
			}

			if(numGeneralTexture == 0)
			{
				vecMaterial_[i].pFileName = "NULL";

				const FbxImplementation* pImplementation = GetImplementation(material, FBXSDK_IMPLEMENTATION_CGFX);

				if(pImplementation != NULL)
				{
					const FbxBindingTable* pRootTable = pImplementation->GetRootTable();

					size_t entryCount = pRootTable->GetEntryCount();

					for(unsigned int clusterCnt = 0; clusterCnt < entryCount; clusterCnt++)
					{
						const char* entryName = pRootTable->GetEntry(clusterCnt).GetSource();

						FbxProperty property = material->RootProperty.FindHierarchical(entryName);

						int fileTextureCount = property.GetSrcObjectCount<FbxFileTexture>();

						for(int k = 0; k < fileTextureCount; k++)
						{
							FbxFileTexture* pFileTexture = property.GetSrcObject<FbxFileTexture>(k);

							if(pFileTexture != NULL)
							{
								// materialはKFbxSurfaceMaterialオブジェクト
								std::string textureName = pFileTexture->GetName();

								Texture* pTex = new Texture;
								vecTexPtr_.push_back(pTex);
								string s = pFileName;
								AddDirectoryPath(textureName, s);
								vecTexPtr_[vecTexPtr_.size() - 1]->LoadImageFromFile(textureName.c_str());

								vecMaterial_[i].pFileName = textureName.c_str();
								vecMaterial_[i].pTexture = pTex;
							}
						}
					}
				}
			}
		} else
		{
			vecMaterial_[i].pFileName = "NULL";
			vecMaterial_[i].pTexture = nullptr;
		}
	}


	//------------------------------------
	//メッシュ
	//------------------------------------
	for(unsigned int meshCnt = 0; meshCnt < meshSize_; meshCnt++)
	{
		mesh = pFbxScene_->GetSrcObject<FbxMesh>(meshCnt);
		if(mesh != NULL)
		{
			//頂点//////////////////////////////////////////////////
			vecMesh_[meshCnt].nNumPolygon = mesh->GetPolygonCount();//総ポリゴン数
			//m_pMesh[meshCnt].pBoneIndex = new VECTOR4[m_pMesh[meshCnt].nNumVertex];
			//m_pMesh[meshCnt].pWeight = new VECTOR4[m_pMesh[meshCnt].nNumVertex];
			//for(int i = 0; i < 4; i++)
			//{
			//	m_pMesh[meshCnt].boneIndex[i] = new int[m_pMesh[meshCnt].nNumVertex];
			//	m_pMesh[meshCnt].weight[i] = new float[m_pMesh[meshCnt].nNumVertex];
			//}

			vecMesh_[meshCnt].nNumVertex = mesh->GetControlPointsCount();//頂点数

			FbxVector4* src = mesh->GetControlPoints();//頂点座標配列

			vecMesh_[meshCnt].vecVd.resize(vecMesh_[meshCnt].nNumVertex);

			for(unsigned int i = 0; i < vecMesh_[meshCnt].nNumVertex; ++i)
			{
				vecMesh_[meshCnt].vecVd[i].pos.x = (float)src[i][0];
				vecMesh_[meshCnt].vecVd[i].pos.y = (float)src[i][1];
				vecMesh_[meshCnt].vecVd[i].pos.z = (float)src[i][2];
				vecMesh_[meshCnt].vecVd[i].color.r = 1.0f;
				vecMesh_[meshCnt].vecVd[i].color.g = 1.0f;
				vecMesh_[meshCnt].vecVd[i].color.b = 1.0f;
				vecMesh_[meshCnt].vecVd[i].color.a = 1.0f;
			}

			vecMesh_[meshCnt].nNumIndex = mesh->GetPolygonVertexCount();//ポリゴン頂点インデックス

			vecMesh_[meshCnt].vecIndexNumber.resize(vecMesh_[meshCnt].nNumIndex);

			int* IndexVertices = mesh->GetPolygonVertices();
			for(unsigned int i = 0; i < vecMesh_[meshCnt].nNumIndex; ++i)
			{
				vecMesh_[meshCnt].vecIndexNumber[i] = (unsigned short)IndexVertices[i];
			}

			//法線//////////////////////////////////////////////////
			int LayerCount = mesh->GetLayerCount();
			for(int i = 0; i < LayerCount; ++i)
			{
				FbxLayer* layer = mesh->GetLayer(i);
				FbxLayerElementNormal* normalLayer = mesh->GetElementNormal(i);
				if(normalLayer == 0)
				{
					continue;//法線ない
				}

				auto pElementTangent = mesh->GetElementTangent(i);
				auto pElementBinormal = mesh->GetElementBinormal(i);

				//法線の数・インデックス
				unsigned int normalCount = normalLayer->GetDirectArray().GetCount();

				vector<Vector3D> normals;
				normals.resize(normalCount);

				//for(unsigned int count = 0; count < normalCount; ++count)
				//{
				//	normals[count].x = (float)normalLayer->GetDirectArray()[count][0];
				//	normals[count].y = (float)normalLayer->GetDirectArray()[count][2];
				//	normals[count].z = (float)normalLayer->GetDirectArray()[count][1];
				//}
				//マッピングモード・リファレンスモード取得
				FbxLayerElement::EMappingMode mappingMode = normalLayer->GetMappingMode();
				FbxLayerElement::EReferenceMode referencegMode = normalLayer->GetReferenceMode();

				if(mappingMode == FbxLayerElement::eByPolygonVertex)
				{
					if(referencegMode == FbxLayerElement::eDirect)
					{
						for(unsigned int clusterCnt = 0; clusterCnt < normalCount; clusterCnt++)
						{
							normals[clusterCnt].x = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[0];
							normals[clusterCnt].y = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[1];
							normals[clusterCnt].z = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[2];
						}
					}
				} else if(mappingMode == FbxLayerElement::eByControlPoint)
				{
					if(referencegMode == FbxLayerElement::eDirect)
					{
						for(unsigned int clusterCnt = 0; clusterCnt < normalCount; clusterCnt++)
						{
							normals[clusterCnt].x = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[0];
							normals[clusterCnt].y = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[1];
							normals[clusterCnt].z = (float)normalLayer->GetDirectArray().GetAt(clusterCnt)[2];
						}
					}
				}

				for(unsigned int i = 0; i < normalCount; ++i)
				{
					vecMesh_[meshCnt].vecVd[vecMesh_[meshCnt].vecIndexNumber[i]].nor.x = normals[i].x;
					vecMesh_[meshCnt].vecVd[vecMesh_[meshCnt].vecIndexNumber[i]].nor.y = normals[i].y;
					vecMesh_[meshCnt].vecVd[vecMesh_[meshCnt].vecIndexNumber[i]].nor.z = normals[i].z;
				}

				//UV//////////////////////////////////////////////////
				FbxLayerElementUV* layerUV = layer->GetUVs();
				if(layerUV == 0)
				{
					continue;
				}

				//UV取得
				// UVの数・インデックス
				int nNumUv = layerUV->GetDirectArray().GetCount();
				int nNumIndexUv = layerUV->GetIndexArray().GetCount();
				vecMesh_[meshCnt].nNumUv = layerUV->GetDirectArray().GetCount();
				vecMesh_[meshCnt].nNumIndexUv = layerUV->GetIndexArray().GetCount();

				vector<Vector2D> texies;
				texies.resize(nNumUv);

				vecMesh_[meshCnt].vecTexIndex.resize(vecMesh_[meshCnt].nNumIndexUv);

				vecMesh_[meshCnt].vecUvIndexNumber.resize(vecMesh_[meshCnt].nNumIndexUv);

				// 頂点に格納されている全UVセットを名前で取得
				FbxStringList uvsetName;
				mesh->GetUVSetNames(uvsetName);

				// マッピングモード・リファレンスモード別にUV取得
				FbxLayerElement::EMappingMode	mappingModeUV = layerUV->GetMappingMode();
				FbxLayerElement::EReferenceMode	refModeUV = layerUV->GetReferenceMode();

				if(mappingModeUV == FbxLayerElement::eByPolygonVertex)
				{
					if(refModeUV == FbxLayerElement::eDirect)
					{
						// 直接取得
						for(int i = 0; i < nNumUv; ++i)
						{
							vecMesh_[meshCnt].vecVd [i].tex.x = (float)layerUV->GetDirectArray().GetAt(i)[0];
							vecMesh_[meshCnt].vecVd[i].tex.y = 1.0f - (float)layerUV->GetDirectArray().GetAt(i)[1];
						}
					} else if(refModeUV == FbxLayerElement::eIndexToDirect)
					{
						int lPolygonsCount = mesh->GetPolygonCount();
						// 頂点バッファのループ
						for(unsigned int i = 0; i < vecMesh_[meshCnt].nNumVertex; i++)
						{
							UINT UVIndex = 0;
							// ポリゴンごとのループ
							for(int j = 0; j < lPolygonsCount; j++)
							{
								// ポリゴン数を取得
								int lPolygonSize = mesh->GetPolygonSize(j);
								// １ポリゴン内の頂点ごとのループ
								for(int k = 0; k < lPolygonSize; k++)
								{
									// インデックスが同じなので処理対象
									if(i == mesh->GetPolygonVertex(j, k))
									{
										// インデックスバッファからインデックスを取得する
										int lUVIndex = layerUV->GetIndexArray().GetAt(UVIndex);

										// 取得したインデックスから UV を取得する
										FbxVector2 lVec2 = layerUV->GetDirectArray().GetAt(lUVIndex);

										// UV値セット

										vecMesh_[meshCnt].vecVd[i].tex.x = (float)layerUV->GetDirectArray().GetAt(lUVIndex)[0];
										vecMesh_[meshCnt].vecVd[i].tex.y = 1.0f - (float)layerUV->GetDirectArray().GetAt(lUVIndex)[1];
									}
									UVIndex++;
								}
							}
						}
						//for(int i = 0; i < nNumUv; i++)
						//{
						//	texies[i].x = (float)layerUV->GetDirectArray().GetAt(i)[0];
						//	texies[i].y = (float)layerUV->GetDirectArray().GetAt(i)[1];
						//}
						//// インデックスから取得
						//for(unsigned int i = 0; i < vecMesh_[meshCnt].nNumIndexUv; ++i) {
						//	vecMesh_[meshCnt].vecUvIndexNumber[i] = layerUV->GetIndexArray().GetAt(i);
						//	vecMesh_[meshCnt].vecTexIndex[i].x = (float)layerUV->GetDirectArray().GetAt(vecMesh_[meshCnt].vecUvIndexNumber[i])[0];
						//	vecMesh_[meshCnt].vecTexIndex[i].y = (float)layerUV->GetDirectArray().GetAt(vecMesh_[meshCnt].vecUvIndexNumber[i])[1];

						//}
						//for(unsigned int i = 0; i < vecMesh_[meshCnt].nNumIndexUv; ++i)
						//{
						//	vecMesh_[meshCnt].vecVd[vecMesh_[meshCnt].vecIndexNumber[i]].tex.x = max(0.0f, min(1.0f, vecMesh_[meshCnt].vecTexIndex[i].x));
						//	vecMesh_[meshCnt].vecVd[vecMesh_[meshCnt].vecIndexNumber[i]].tex.y = max(0.0f, min(1.0f, 1.0f - vecMesh_[meshCnt].vecTexIndex[i].y));
						//}
					}
				}

			}

			src = NULL;
			IndexVertices = NULL;

			//マテリアル紐付け//////////////////////////////////////////////////
			FbxLayerElementArrayTemplate<int>* matIndexs;
			char* pUvName = {};
			if(mesh->GetMaterialIndices(&matIndexs) && matIndexs->GetCount() > 0)
			{
				auto material = mesh->GetNode()->GetMaterial(matIndexs->GetAt(0));
				pUvName = (char*)material->GetName();
			}


			for(int i = 0; i < nNumMaterial; i++)
			{
				if(vecMaterial_[i].pMaterialName == pUvName)
				{
					vecMesh_[meshCnt].color = vecMaterial_[i].color;
					if(vecMaterial_[i].pFileName != "NULL")
					{
						vecMesh_[meshCnt].pTextureFileName = (char*)vecMaterial_[i].pFileName;
						vecMesh_[meshCnt].pTexture = vecMaterial_[i].pTexture;
					} else{
						vecMesh_[meshCnt].pTextureFileName = NULL;
						vecMesh_[meshCnt].pTexture = nullptr;
					}
					break;
				}
			}

			////メッシュスキン数の取得
			//int nNumDeformer = mesh->GetDeformerCount(FbxDeformer::eSkin);

			//m_pMesh[meshCnt].pCluster = NULL;

			//for(int i = 0; i < nNumDeformer; i++)
			//{
			//	//スキン情報を取得
			//	FbxSkin* pSkin = (FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin);

			//	//スキンのクラスター(ボーン)の数を取得する。
			//	int nNumCluster = pSkin->GetClusterCount();

			//	m_pMesh[meshCnt].pCluster = new Cluster[nNumCluster];

			//	m_pMesh[meshCnt].nNumCluster = nNumCluster;

			//	m_pMesh[meshCnt].pWeight = new VECTOR4[m_pMesh[meshCnt].nNumVertex];
			//	m_pMesh[meshCnt].pBoneIndex = new VECTOR4[m_pMesh[meshCnt].nNumVertex];

			//	for(int k = 0; k < m_pMesh[meshCnt].nNumVertex; k++)
			//	{
			//		for(int i = 0; i < 4; i++)
			//		{
			//			m_pMesh[meshCnt].weight[i][k] = 0;
			//			m_pMesh[meshCnt].boneIndex[i][k] = 0;

			//			m_pMesh[meshCnt].pWeight[i] = VECTOR4(0, 0, 0, 0);
			//			m_pMesh[meshCnt].pBoneIndex[i] = VECTOR4(0, 0, 0, 0);
			//		}
			//	}

			//	bool bUse[4][5000] = { false };
			//	int BoneCnt[5000] = { 0 };

			//	for(int boneCnt = 0; boneCnt < nNumCluster; boneCnt++)
			//	{
			//		FbxCluster* cluster = pSkin->GetCluster(boneCnt);

			//		//クラスター頂点インデックスとウェイト数を取得
			//		int controlPointIndexCount = cluster->GetControlPointIndicesCount();
			//		int* pointAry = cluster->GetControlPointIndices();
			//		double* weightAry = cluster->GetControlPointWeights();

			//		m_pMesh[meshCnt].pCluster[boneCnt].Index = new int[controlPointIndexCount];
			//		m_pMesh[meshCnt].pCluster[boneCnt].weight = new float[controlPointIndexCount];

			//		for(int indexCnt = 0; indexCnt < controlPointIndexCount; indexCnt++)
			//		{
			//			//頂点インデックスとウェイトの配列取得
			//			m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt] = pointAry[indexCnt];
			//			m_pMesh[meshCnt].pCluster[boneCnt].weight[indexCnt] = (float)weightAry[indexCnt];

			//			if(bUse[BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]]][m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]])
			//			{
			//				BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] ++;
			//			}

			//			if(BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] < 4)
			//			{
			//				m_pMesh[meshCnt].weight[BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]]][m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] = (float)weightAry[indexCnt];

			//				m_pMesh[meshCnt].boneIndex[BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]]][m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] = boneCnt;

			//				bUse[BoneCnt[m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]]][m_pMesh[meshCnt].pCluster[boneCnt].Index[indexCnt]] = true;
			//			}
			//		}


			//		for(int j = 0; j < m_pMesh[meshCnt].nNumVertex; j++)
			//		{
			//			m_pMesh[meshCnt].pBoneIndex[j] = VECTOR4(m_pMesh[meshCnt].boneIndex[0][j], m_pMesh[meshCnt].boneIndex[1][j], m_pMesh[meshCnt].boneIndex[2][j], m_pMesh[meshCnt].boneIndex[3][j]);
			//			m_pMesh[meshCnt].pWeight[j] = VECTOR4(m_pMesh[meshCnt].weight[0][j], m_pMesh[meshCnt].weight[1][j], m_pMesh[meshCnt].weight[2][j], m_pMesh[meshCnt].weight[3][j]);
			//		}

			//		m_pMesh[meshCnt].pCluster[boneCnt].pMatrix = new XMMATRIX * [m_nNumAnime];

			//		for(int animeCnt = 0; animeCnt < m_nNumAnime; animeCnt++)
			//		{
			//			FbxAnimStack* pStack = pScene->GetSrcObject<FbxAnimStack>(animeCnt);
			//			pScene->SetCurrentAnimationStack(pStack);
			//			m_pMesh[meshCnt].pCluster[boneCnt].pMatrix[animeCnt] = new XMMATRIX[m_pAnime[animeCnt].nEndTime];
			//			FbxAMatrix initMat;
			//			FbxAMatrix Mat;

			//			//初期姿勢行列を取得
			//			cluster->GetTransformLinkMatrix(initMat);
			//			cluster->GetTransformMatrix(Mat);

			//			XMVECTOR Determinant;
			//			XMMATRIX init = XMMatrixInverse(&Determinant, SetMatrix(initMat));
			//			XMMATRIX mat = SetMatrix(Mat);

			//			//クラスターからノード取得
			//			FbxNode* node = cluster->GetLink();

			//			FbxTime period;
			//			period.SetTime(0, 0, 0, 1, 0, FbxTime::eFrames24);

			//			for(int frameCnt = 0; frameCnt < m_pAnime[animeCnt].nEndTime; frameCnt++)
			//			{
			//				m_pMesh[meshCnt].pCluster[boneCnt].pMatrix[animeCnt][frameCnt] = init * SetMatrix(node->EvaluateGlobalTransform(frameCnt * period.Get()));
			//			}
			//		}
			//	}

			//}

			//FbxNode* n = mesh->GetNode();
			////if (meshCnt != 6)
			//{
			//	m_pMesh[meshCnt].LclPos.x = n->LclTranslation.Get().mData[0];
			//	m_pMesh[meshCnt].LclPos.y = n->LclTranslation.Get().mData[1];
			//	m_pMesh[meshCnt].LclPos.z = n->LclTranslation.Get().mData[2];

			//	m_pMesh[meshCnt].LclRot.x = n->LclRotation.Get().mData[0];
			//	m_pMesh[meshCnt].LclRot.y = n->LclRotation.Get().mData[1];
			//	m_pMesh[meshCnt].LclRot.z = n->LclRotation.Get().mData[2];

			//	m_pMesh[meshCnt].LclScl.x = n->LclScaling.Get().mData[0];
			//	m_pMesh[meshCnt].LclScl.y = n->LclScaling.Get().mData[1];
			//	m_pMesh[meshCnt].LclScl.z = n->LclScaling.Get().mData[2];
			//}
		}
	}













	//int meshCount = pFbxScene_->GetSrcObjectCount<FbxMesh>();

	////ObjSubset subset;
	////ZeroMemory(&subset, sizeof(subset));
	////subset.faceStart = vecIndex_.size();
	////vecSubset_.push_back(subset);

	//SetupNode(pFbxScene_->GetRootNode(), "", 0, pFileName);

	// サイズの格納
	vertexSize_ = vecVertex_.size();
	indexSize_ = vecIndex_.size();
	texPtrSize_ = vecTexPtr_.size();

	vecVertexBufferPtr_.resize(meshSize_);
	vecIndexBufferPtr_.resize(meshSize_);

	for(unsigned int meshCnt = 0; meshCnt < meshSize_; meshCnt++)
	{
		{
			// 頂点バッファを作成
			D3D11_BUFFER_DESC bd;
			bd.ByteWidth = sizeof(MeshVertexData) * vecMesh_[meshCnt].nNumVertex;
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bd.MiscFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = &vecMesh_[meshCnt].vecVd[0];
			if(FAILED(gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &vecVertexBufferPtr_[meshCnt]))){ return false; }
		}

		{
			// インデックスバッファを作成
			D3D11_BUFFER_DESC bd;
			bd.ByteWidth = sizeof(unsigned short) * vecMesh_[meshCnt].nNumIndex;
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = &vecMesh_[meshCnt].vecIndexNumber[0];
			InitData.SysMemPitch = 0;
			InitData.SysMemSlicePitch = 0;
			if(FAILED(gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &vecIndexBufferPtr_[meshCnt]))){ return false; }

		}
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
		for(unsigned int meshCnt = 0; meshCnt < meshSize_; meshCnt++)
		{
			SafeRelease(vecIndexBufferPtr_[meshCnt]);
			SafeRelease(vecVertexBufferPtr_[meshCnt]);
		}
	}

	pVertexBuffer_ = nullptr;
	pIndexBuffer_ = nullptr;
	vertexSize_ = 0;
	indexSize_ = 0;
	materialSize_ = 0;
	texPtrSize_ = 0;
	bStorage_ = false;
	vector<MeshVertexData>().swap(vecVertex_);
	vector<FbxMaterial>().swap(vecMaterial_);
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
		//|| !GetVertexBufferPtr()
		//|| GetVertexSize() <= 0
		//|| !GetIndexBufferPtr()
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

	////バーテックスバッファをセット
	//ID3D11Buffer* pVBuf = GetVertexBufferPtr();
	//UINT stride = sizeof(MeshVertexData);
	//UINT offset = 0;
	//gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);///

	//																	   //インデックスバッファをセット
	//gm.GetContextPtr()->IASetIndexBuffer(GetIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);///

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

	for(unsigned int meshCnt = 0; meshCnt < meshSize_; meshCnt++)
	{
		//バーテックスバッファをセット
		ID3D11Buffer* pVBuf = vecVertexBufferPtr_[meshCnt];
		UINT stride = sizeof(MeshVertexData);
		UINT offset = 0;
		gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);///

																			   //インデックスバッファをセット
		gm.GetContextPtr()->IASetIndexBuffer(vecIndexBufferPtr_[meshCnt], DXGI_FORMAT_R16_UINT, 0);///

		Texture* pTexture = vecMesh_[meshCnt].pTexture;
		if(pTexture){
			ID3D11ShaderResourceView* pTexView = pTexture->GetTextureViewPtr();
			if(pTexView){
				gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
			}
			if(pTexView){
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
			} else{
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
			}
		} else{
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
		}

		gm.GetContextPtr()->DrawIndexed(vecMesh_[meshCnt].nNumIndex, 0, 0);///
	}

	//// サブセット毎に描画
	//if(subsetSize_ <= 1){///
	//	ID3D11ShaderResourceView* pTexView = nullptr;
	//	if(texPtrSize_ > 0){///
	//		pTexView = vecTexPtr_[0]->GetTextureViewPtr();///
	//	}
	//	if(pTexView){
	//		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	//	}
	//	if(pTexView){
	//		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	//	} else{
	//		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	//	}
	//	gm.GetContextPtr()->Draw(GetVertexSize(), 0);///
	//} else{
	//	for(unsigned int i = 1; i < subsetSize_; ++i){///

	//		ID3D11ShaderResourceView* pTexView = nullptr;
	//		if(texPtrSize_ > i){///
	//			pTexView = vecTexPtr_[i - 1]->GetTextureViewPtr();///
	//		}
	//		if(pTexView){
	//			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	//		}
	//		if(pTexView){
	//			gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	//		} else{
	//			gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	//		}
	//		gm.GetContextPtr()->Draw(GetSubsetPtr()[i].faceStart - GetSubsetPtr()[i - 1].faceStart, GetSubsetPtr()[i - 1].faceStart);///
	//	}
	//	ID3D11ShaderResourceView* pTexView = nullptr;
	//	if(texPtrSize_ == subsetSize_ - 1){///
	//		pTexView = vecTexPtr_[subsetSize_ - 1]->GetTextureViewPtr();///
	//	}
	//	if(pTexView){
	//		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	//	}
	//	if(pTexView){
	//		gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
	//	} else{
	//		gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
	//	}
	//	gm.GetContextPtr()->Draw(GetVertexSize() - GetSubsetPtr()[subsetSize_ - 1].faceStart, GetSubsetPtr()[subsetSize_ - 1].faceStart);///
	//}
}
