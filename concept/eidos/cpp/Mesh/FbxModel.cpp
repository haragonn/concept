
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
// ���[�j���O�������Ƃ�����
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
		//--- ���b�V������m�[�h���擾 ---//
		FbxNode* node = pNode;

		//--- �}�e���A���̐����擾 ---//
		int materialCount = node->GetMaterialCount();

		//--- �}�e���A���̐������J��Ԃ� ---//
		for(int i = 0; materialCount > i; i++) {

			//--- �}�e���A�����擾 ---//
			FbxSurfaceMaterial* material = node->GetMaterial(i);
			FbxProperty prop = material->FindProperty(FbxSurfaceMaterial::sDiffuse);

			//--- FbxLayeredTexture �̐����擾 ---//
			int layeredTextureCount = prop.GetSrcObjectCount<FbxLayeredTexture>();

			//--- �A�^�b�`���ꂽ�e�N�X�`���� FbxLayeredTexture �̏ꍇ ---//
			if(0 < layeredTextureCount) {

				//--- �A�^�b�`���ꂽ�e�N�X�`���̐������J��Ԃ� ---//
				for(int j = 0; layeredTextureCount > j; j++) {

					//--- �e�N�X�`�����擾 ---//
					FbxLayeredTexture* layeredTexture = prop.GetSrcObject<FbxLayeredTexture>(j);
					//--- ���C���[�����擾 ---//
					int textureCount = layeredTexture->GetSrcObjectCount<FbxFileTexture>();

					//--- ���C���[�������J��Ԃ� ---//
					for(int k = 0; textureCount > k; k++) {
						//--- �e�N�X�`�����擾 ---//
						FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(k);

						if(texture) {
							//--- �e�N�X�`�������擾 ---//
							std::string textureName = texture->GetName();
							//std::string textureName = texture->GetRelativeFileName();

							//--- UVSet�����擾 ---//
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
				//--- �e�N�X�`�������擾 ---//
				int fileTextureCount = prop.GetSrcObjectCount<FbxFileTexture>();

				if(0 < fileTextureCount) {
					//--- �e�N�X�`���̐������J��Ԃ� ---//
					for(int j = 0; fileTextureCount > j; j++) {
						//--- �e�N�X�`�����擾 ---//
						FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(j);
						if(texture) {
							//--- �e�N�X�`�������擾 ---//
							std::string textureName = texture->GetName();
							//std::string textureName = texture->GetRelativeFileName();

							//--- UVSet�����擾 ---//
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
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// ���ɓǂݍ��ݍς݂Ȃ�I��

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

	// ����DirectX�p��
	FbxAxisSystem SceneAxisSystem = pFbxScene_->GetGlobalSettings().GetAxisSystem();
	if(SceneAxisSystem != FbxAxisSystem::DirectX)
	{
		FbxAxisSystem::DirectX.ConvertScene(pFbxScene_);
	}

	FbxGeometryConverter geometryConverter(pFbxManager_);

	// �O�p�|���S����
	geometryConverter.Triangulate(pFbxScene_, true);

	// �k�ރ|���S���̍폜
	geometryConverter.RemoveBadPolygonsFromMeshes(pFbxScene_);

	// �}�e���A�����ƂɃ��b�V���𕪗�
	geometryConverter.SplitMeshesPerMaterial(pFbxScene_, true);

	//------------------------------------
	//FBXSDK�ǂݍ���
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
	//�}�e���A��
	//------------------------------------
	for(int i = 0; i < nNumMaterial; i++)
	{
		FbxSurfaceLambert* lambert;
		FbxSurfacePhong* phong;

		FbxSurfaceMaterial* material = pFbxScene_->GetMaterial(i);
		// material��KFbxSurfaceMaterial�I�u�W�F�N�g
		vecMaterial_[i].pMaterialName = (char*)material->GetName();

		// Lambert��Phong��
		if(material->GetClassId().Is(FbxSurfaceLambert::ClassId))
		{
			// Lambert�Ƀ_�E���L���X�g
			lambert = (FbxSurfaceLambert*)material;

			FbxDouble3 diffuse = lambert->Diffuse;
			vecMaterial_[i].color.r = (float)diffuse[0];
			vecMaterial_[i].color.g = (float)diffuse[1];
			vecMaterial_[i].color.b = (float)diffuse[2];
			vecMaterial_[i].color.a = (float)diffuse[3];
		} else if(material->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			// Phong�Ƀ_�E���L���X�g
			phong = (FbxSurfacePhong*)material;
		}


		// �f�B�t���[�Y�v���p�e�B������
		FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		vecMesh_[i].pTextureFileName = NULL;
		// �v���p�e�B�������Ă��郌�C���[�h�e�N�X�`���̖������`�F�b�N
		int layerNum = property.GetSrcObjectCount<FbxLayeredTexture>();
		// ���C���[�h�e�N�X�`����������Βʏ�e�N�X�`��
		if(layerNum == 0)
		{
			// �ʏ�e�N�X�`���̖������`�F�b�N
			int numGeneralTexture = property.GetSrcObjectCount<FbxFileTexture>();
			numGeneralTexture = numGeneralTexture > 0 ? 1 : 0;///
			vecMesh_[i].pTextureFileName = NULL;
			// �e�e�N�X�`���ɂ��ăe�N�X�`�������Q�b�g
			for(int clusterCnt = 0; clusterCnt < numGeneralTexture; ++clusterCnt)
			{
				// i�Ԗڂ̃e�N�X�`���I�u�W�F�N�g�擾
				FbxFileTexture* texture = FbxCast<FbxFileTexture>(property.GetSrcObject<FbxFileTexture>(clusterCnt));

				// material��KFbxSurfaceMaterial�I�u�W�F�N�g
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
								// material��KFbxSurfaceMaterial�I�u�W�F�N�g
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
	//���b�V��
	//------------------------------------
	for(unsigned int meshCnt = 0; meshCnt < meshSize_; meshCnt++)
	{
		mesh = pFbxScene_->GetSrcObject<FbxMesh>(meshCnt);
		if(mesh != NULL)
		{
			//���_//////////////////////////////////////////////////
			vecMesh_[meshCnt].nNumPolygon = mesh->GetPolygonCount();//���|���S����
			//m_pMesh[meshCnt].pBoneIndex = new VECTOR4[m_pMesh[meshCnt].nNumVertex];
			//m_pMesh[meshCnt].pWeight = new VECTOR4[m_pMesh[meshCnt].nNumVertex];
			//for(int i = 0; i < 4; i++)
			//{
			//	m_pMesh[meshCnt].boneIndex[i] = new int[m_pMesh[meshCnt].nNumVertex];
			//	m_pMesh[meshCnt].weight[i] = new float[m_pMesh[meshCnt].nNumVertex];
			//}

			vecMesh_[meshCnt].nNumVertex = mesh->GetControlPointsCount();//���_��

			FbxVector4* src = mesh->GetControlPoints();//���_���W�z��

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

			vecMesh_[meshCnt].nNumIndex = mesh->GetPolygonVertexCount();//�|���S�����_�C���f�b�N�X

			vecMesh_[meshCnt].vecIndexNumber.resize(vecMesh_[meshCnt].nNumIndex);

			int* IndexVertices = mesh->GetPolygonVertices();
			for(unsigned int i = 0; i < vecMesh_[meshCnt].nNumIndex; ++i)
			{
				vecMesh_[meshCnt].vecIndexNumber[i] = (unsigned short)IndexVertices[i];
			}

			//�@��//////////////////////////////////////////////////
			int LayerCount = mesh->GetLayerCount();
			for(int i = 0; i < LayerCount; ++i)
			{
				FbxLayer* layer = mesh->GetLayer(i);
				FbxLayerElementNormal* normalLayer = mesh->GetElementNormal(i);
				if(normalLayer == 0)
				{
					continue;//�@���Ȃ�
				}

				auto pElementTangent = mesh->GetElementTangent(i);
				auto pElementBinormal = mesh->GetElementBinormal(i);

				//�@���̐��E�C���f�b�N�X
				unsigned int normalCount = normalLayer->GetDirectArray().GetCount();

				vector<Vector3D> normals;
				normals.resize(normalCount);

				//for(unsigned int count = 0; count < normalCount; ++count)
				//{
				//	normals[count].x = (float)normalLayer->GetDirectArray()[count][0];
				//	normals[count].y = (float)normalLayer->GetDirectArray()[count][2];
				//	normals[count].z = (float)normalLayer->GetDirectArray()[count][1];
				//}
				//�}�b�s���O���[�h�E���t�@�����X���[�h�擾
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

				//UV�擾
				// UV�̐��E�C���f�b�N�X
				int nNumUv = layerUV->GetDirectArray().GetCount();
				int nNumIndexUv = layerUV->GetIndexArray().GetCount();
				vecMesh_[meshCnt].nNumUv = layerUV->GetDirectArray().GetCount();
				vecMesh_[meshCnt].nNumIndexUv = layerUV->GetIndexArray().GetCount();

				vector<Vector2D> texies;
				texies.resize(nNumUv);

				vecMesh_[meshCnt].vecTexIndex.resize(vecMesh_[meshCnt].nNumIndexUv);

				vecMesh_[meshCnt].vecUvIndexNumber.resize(vecMesh_[meshCnt].nNumIndexUv);

				// ���_�Ɋi�[����Ă���SUV�Z�b�g�𖼑O�Ŏ擾
				FbxStringList uvsetName;
				mesh->GetUVSetNames(uvsetName);

				// �}�b�s���O���[�h�E���t�@�����X���[�h�ʂ�UV�擾
				FbxLayerElement::EMappingMode	mappingModeUV = layerUV->GetMappingMode();
				FbxLayerElement::EReferenceMode	refModeUV = layerUV->GetReferenceMode();

				if(mappingModeUV == FbxLayerElement::eByPolygonVertex)
				{
					if(refModeUV == FbxLayerElement::eDirect)
					{
						// ���ڎ擾
						for(int i = 0; i < nNumUv; ++i)
						{
							vecMesh_[meshCnt].vecVd [i].tex.x = (float)layerUV->GetDirectArray().GetAt(i)[0];
							vecMesh_[meshCnt].vecVd[i].tex.y = 1.0f - (float)layerUV->GetDirectArray().GetAt(i)[1];
						}
					} else if(refModeUV == FbxLayerElement::eIndexToDirect)
					{
						int lPolygonsCount = mesh->GetPolygonCount();
						// ���_�o�b�t�@�̃��[�v
						for(unsigned int i = 0; i < vecMesh_[meshCnt].nNumVertex; i++)
						{
							UINT UVIndex = 0;
							// �|���S�����Ƃ̃��[�v
							for(int j = 0; j < lPolygonsCount; j++)
							{
								// �|���S�������擾
								int lPolygonSize = mesh->GetPolygonSize(j);
								// �P�|���S�����̒��_���Ƃ̃��[�v
								for(int k = 0; k < lPolygonSize; k++)
								{
									// �C���f�b�N�X�������Ȃ̂ŏ����Ώ�
									if(i == mesh->GetPolygonVertex(j, k))
									{
										// �C���f�b�N�X�o�b�t�@����C���f�b�N�X���擾����
										int lUVIndex = layerUV->GetIndexArray().GetAt(UVIndex);

										// �擾�����C���f�b�N�X���� UV ���擾����
										FbxVector2 lVec2 = layerUV->GetDirectArray().GetAt(lUVIndex);

										// UV�l�Z�b�g

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
						//// �C���f�b�N�X����擾
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

			//�}�e���A���R�t��//////////////////////////////////////////////////
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

			////���b�V���X�L�����̎擾
			//int nNumDeformer = mesh->GetDeformerCount(FbxDeformer::eSkin);

			//m_pMesh[meshCnt].pCluster = NULL;

			//for(int i = 0; i < nNumDeformer; i++)
			//{
			//	//�X�L�������擾
			//	FbxSkin* pSkin = (FbxSkin*)mesh->GetDeformer(i, FbxDeformer::eSkin);

			//	//�X�L���̃N���X�^�[(�{�[��)�̐����擾����B
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

			//		//�N���X�^�[���_�C���f�b�N�X�ƃE�F�C�g�����擾
			//		int controlPointIndexCount = cluster->GetControlPointIndicesCount();
			//		int* pointAry = cluster->GetControlPointIndices();
			//		double* weightAry = cluster->GetControlPointWeights();

			//		m_pMesh[meshCnt].pCluster[boneCnt].Index = new int[controlPointIndexCount];
			//		m_pMesh[meshCnt].pCluster[boneCnt].weight = new float[controlPointIndexCount];

			//		for(int indexCnt = 0; indexCnt < controlPointIndexCount; indexCnt++)
			//		{
			//			//���_�C���f�b�N�X�ƃE�F�C�g�̔z��擾
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

			//			//�����p���s����擾
			//			cluster->GetTransformLinkMatrix(initMat);
			//			cluster->GetTransformMatrix(Mat);

			//			XMVECTOR Determinant;
			//			XMMATRIX init = XMMatrixInverse(&Determinant, SetMatrix(initMat));
			//			XMMATRIX mat = SetMatrix(Mat);

			//			//�N���X�^�[����m�[�h�擾
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

	// �T�C�Y�̊i�[
	vertexSize_ = vecVertex_.size();
	indexSize_ = vecIndex_.size();
	texPtrSize_ = vecTexPtr_.size();

	vecVertexBufferPtr_.resize(meshSize_);
	vecIndexBufferPtr_.resize(meshSize_);

	for(unsigned int meshCnt = 0; meshCnt < meshSize_; meshCnt++)
	{
		{
			// ���_�o�b�t�@���쐬
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
			// �C���f�b�N�X�o�b�t�@���쐬
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
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// ���ɓǂݍ��ݍς݂Ȃ�I��

	GraphicManager& gm = GraphicManager::Instance();

	if(!gm.GetContextPtr()){ return false; }

	// tmp�t�@�C�����쐬
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

	// �A�[�J�C�u�t�@�C�����J��
	ArchiveLoader loader;
	if(!loader.Load(pArchiveFileName, pFileName)){ return false; }

	// tmp�t�@�C����������
	FILE* fp;

	if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

	if(fp == NULL) {
		return false;
	} else {
		fwrite(&loader.GetData()[0], loader.GetSize(), sizeof(BYTE), fp);
		fclose(fp);
	}

	// �t�@�C�����J���Ĉꎞ�X�g���[���ɏ�������
	ifstream file;
	file.open(tempFileName, ios::in);
	if(!file.is_open()){ return false; }

	stringstream tmpStream;
	tmpStream << file.rdbuf();

	file.close();

	// tmp�t�@�C����O�̂��ߏ�����
	if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

	if(fp == NULL) {
		return false;
	} else {
		fwrite("", 1, sizeof(BYTE), fp);
		fclose(fp);
	}

	DeleteFileA(tempFileName);	//tmp�t�@�C���폜


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

	// �������ł��Ă��Ȃ���ΏI��
	if(!gm.GetContextPtr()
		|| !om.GetVertexShederPtr()
		//|| !GetVertexBufferPtr()
		//|| GetVertexSize() <= 0
		//|| !GetIndexBufferPtr()
		|| !pCamera){
		return;
	}

	//�萔�o�b�t�@
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

	// �萔�o�b�t�@���e�X�V
	gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

	// �萔�o�b�t�@���Z�b�g
	UINT cb_slot = 1;
	ID3D11Buffer* cb[1] = { om.GetConstBufferPtr() };
	gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);

	////�o�[�e�b�N�X�o�b�t�@���Z�b�g
	//ID3D11Buffer* pVBuf = GetVertexBufferPtr();
	//UINT stride = sizeof(MeshVertexData);
	//UINT offset = 0;
	//gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);///

	//																	   //�C���f�b�N�X�o�b�t�@���Z�b�g
	//gm.GetContextPtr()->IASetIndexBuffer(GetIndexBufferPtr(), DXGI_FORMAT_R16_UINT, 0);///

																							   // ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(om.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ���X�^���C�U�X�e�[�g
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// �f�v�X�X�e���V���X�e�[�g
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(om.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
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
		//�o�[�e�b�N�X�o�b�t�@���Z�b�g
		ID3D11Buffer* pVBuf = vecVertexBufferPtr_[meshCnt];
		UINT stride = sizeof(MeshVertexData);
		UINT offset = 0;
		gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);///

																			   //�C���f�b�N�X�o�b�t�@���Z�b�g
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

	//// �T�u�Z�b�g���ɕ`��
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
