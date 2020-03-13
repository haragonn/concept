#include "../../h/Mesh/PmxModel.h"
#include "../../h/eidosStorage/eidosStorageManager.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Archive/ArchiveLoader.h"
#include "../../../idea/h/Utility/ideaUtility.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

bool GetPMXStringUTF16(std::ifstream& _file, std::wstring& output)
{
	std::array<wchar_t, 512> wBuffer{};
	int textSize;

	_file.read(reinterpret_cast<char*>(&textSize), 4);

	_file.read(reinterpret_cast<char*>(&wBuffer), textSize);
	output = std::wstring(&wBuffer[0], &wBuffer[0] + textSize / 2);

	return true;
}

PmxModel::PmxModel() :
	pVertexBuffer_(nullptr),
	pIndexBuffer_(nullptr),
	bStorage_(false)
{
	vector<BlendVertexData>().swap(vecVertex_);
	vector<unsigned short>().swap(vecIndex_);
	vector<PmxMaterial>().swap(vecMaterial_);
	vector<Texture*>().swap(vecTexPtr_);

	vector<PMXModelData::Vertex>().swap(pmxData_.vertices);
	vector<PMXModelData::Surface>().swap(pmxData_.surfaces);
	vector<string>().swap(pmxData_.texturePaths);
	vector<PMXModelData::Material>().swap(pmxData_.materials);
	vector<PMXModelData::Bone>().swap(pmxData_.bones);
}


PmxModel::~PmxModel()
{
	UnLoad();
}

bool PmxModel::LoadPmxMeshFromFile(const char* pFileName)
{
	if(pVertexBuffer_){ return false; }	// 既に読み込み済みなら終了

	GraphicManager& gm = GraphicManager::Instance();

	if(!gm.GetContextPtr()){ return false; }

	ifstream ifs(pFileName, ios::binary);

	if(!ifs.is_open()){
		SetDebugMessage("PmxMeshLoadError! [%s] をファイルから読み込みられませんでした。\n", pFileName);

		return false;
	}

	string filePath = pFileName;

	string folderPath{ filePath.begin(), filePath.begin() + filePath.rfind('/') + 1 };

	// ヘッダー -------------------------------
	array<byte, 4> pmxHeader = {};
	constexpr array<byte, 4> PMX_MAGIC_NUMBER{ 0x50, 0x4d, 0x58, 0x20 };
	enum HeaderDataIndex
	{
		ENCODING_FORMAT,
		NUMBER_OF_ADD_UV,
		VERTEX_INDEX_SIZE,
		TEXTURE_INDEX_SIZE,
		MATERIAL_INDEX_SIZE,
		BONE_INDEX_SIZE,
		RIGID_BODY_INDEX_SIZE
	};

	for(int i = 0; i < 4; ++i){
		pmxHeader[i] = ifs.get();
	}
	if(pmxHeader != PMX_MAGIC_NUMBER){
		ifs.close();
		return false;
	}

	// ver2.0以外は非対応
	float version = 0.0f;
	ifs.read(reinterpret_cast<char*>(&version), 4);
	if(!XMScalarNearEqual(version, 2.0f, g_XMEpsilon.f[0])){
		ifs.close();
		return false;
	}

	byte hederDataLength = ifs.get();
	if(hederDataLength != 8){
		ifs.close();
		return false;
	}

	array<byte, 8> hederData{};
	for(int i = 0; i < hederDataLength; ++i){
		hederData[i] = ifs.get();
	}

	//UTF-8は非対応
	if(hederData[0] != 0){
		ifs.close();
		return false;
	}

	unsigned int arrayLength = 0;

	for(int i = 0; i < 4; ++i){
		ifs.read(reinterpret_cast<char*>(&arrayLength), 4);

		for(unsigned int j = 0; j < arrayLength; ++j){
			ifs.get();
		}
	}

	// 頂点 -----------------------------------
	using Vertex = PMXModelData::Vertex;
	int numberOfVertex = 0;
	ifs.read(reinterpret_cast<char*>(&numberOfVertex), 4);
	vertexSize_ = numberOfVertex;
	pmxData_.vertices.resize(numberOfVertex);

	for(int i = 0; i < numberOfVertex; ++i)
	{
		ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].position), 12);
		ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].normal), 12);
		ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].uv), 8);

		if(hederData[NUMBER_OF_ADD_UV] != 0){
			for(int j = 0; j < hederData[NUMBER_OF_ADD_UV]; ++j){
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].additionalUV[j]), 16);
			}
		}

		const byte weightMethod = ifs.get();

		switch(weightMethod){
		case Vertex::Weight::BDEF1:
			pmxData_.vertices[i].weight.type = Vertex::Weight::BDEF1;
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			pmxData_.vertices[i].weight.born2 = PMXModelData::NO_DATA_FLAG;
			pmxData_.vertices[i].weight.born3 = PMXModelData::NO_DATA_FLAG;
			pmxData_.vertices[i].weight.born4 = PMXModelData::NO_DATA_FLAG;
			pmxData_.vertices[i].weight.weight1 = 1.0f;
			break;

		case Vertex::Weight::BDEF2:
			pmxData_.vertices[i].weight.type = Vertex::Weight::BDEF2;
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
			pmxData_.vertices[i].weight.born3 = PMXModelData::NO_DATA_FLAG;
			pmxData_.vertices[i].weight.born4 = PMXModelData::NO_DATA_FLAG;
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight1), 4);
			pmxData_.vertices[i].weight.weight2 = 1.0f - pmxData_.vertices[i].weight.weight1;
			break;

		case Vertex::Weight::BDEF4:
			pmxData_.vertices[i].weight.type = Vertex::Weight::BDEF4;
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born3), hederData[BONE_INDEX_SIZE]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born4), hederData[BONE_INDEX_SIZE]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight1), 4);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight2), 4);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight3), 4);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight4), 4);
			break;

		case Vertex::Weight::SDEF:
			pmxData_.vertices[i].weight.type = Vertex::Weight::SDEF;
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
			pmxData_.vertices[i].weight.born3 = PMXModelData::NO_DATA_FLAG;
			pmxData_.vertices[i].weight.born4 = PMXModelData::NO_DATA_FLAG;
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight1), 4);
			pmxData_.vertices[i].weight.weight2 = 1.0f - pmxData_.vertices[i].weight.weight1;
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.c), 12);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.r0), 12);
			ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.r1), 12);
			break;

		default:
			ifs.close();
			return false;
		}

		ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].edgeMagnif), 4);

		if(pmxData_.vertices[i].weight.born1 == PMXModelData::NO_DATA_FLAG){
			ifs.close();
			return false;
		}
	}

	// 面  ------------------------------------
	int numOfSurface = 0;
	ifs.read(reinterpret_cast<char*>(&numOfSurface), 4);
	pmxData_.surfaces.resize(numOfSurface);
	indexSize_ = numOfSurface;

	for(int i = 0; i < numOfSurface; ++i){
		ifs.read(reinterpret_cast<char*>(&pmxData_.surfaces[i].vertexIndex), hederData[VERTEX_INDEX_SIZE]);

		if(pmxData_.surfaces[i].vertexIndex == PMXModelData::NO_DATA_FLAG || pmxData_.surfaces[i].vertexIndex == PMXModelData::NO_DATA_FLAG || pmxData_.surfaces[i].vertexIndex == PMXModelData::NO_DATA_FLAG){
			ifs.close();
			return false;
		}
	}

	// テクスチャ -----------------------------
	int numOfTexture = 0;
	ifs.read(reinterpret_cast<char*>(&numOfTexture), 4);

	texPtrSize_ = numOfTexture;

	pmxData_.texturePaths.resize(numOfTexture);

	// テクスチャ読み込み
	wstring texturePath{};
	for(int i = 0; i < numOfTexture; ++i){
		GetPMXStringUTF16(ifs, texturePath);

		string t = WStringToString(texturePath);
		string s = folderPath;
		AddDirectoryPath(t, s);

		pmxData_.texturePaths[i] = t;

		Texture* pTex = new Texture;
		pTex->LoadImageFromFile(t.c_str());
		vecTexPtr_.push_back(pTex);
	}

	// マテリアル -----------------------------
	int numOfMaterial = 0;
	ifs.read(reinterpret_cast<char*>(&numOfMaterial), 4);

	materialSize_ = numOfMaterial;

	pmxData_.materials.resize(numOfMaterial);

	for(int i = 0; i < numOfMaterial; ++i){
		for(int j = 0; j < 2; ++j){
			ifs.read(reinterpret_cast<char*>(&arrayLength), 4);
			for(unsigned int k = 0; k < arrayLength; ++k){ ifs.get(); }
		}

		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].diffuse), 16);
		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].specular), 12);
		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].specularity), 4);
		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].ambient), 12);

		ifs.get();

		for(int j = 0; j < 16; ++j){ ifs.get(); }

		for(int j = 0; j < 4; ++j){ ifs.get(); }

		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].colorMapTextureIndex), hederData[TEXTURE_INDEX_SIZE]);

		for(unsigned char j = 0; j < hederData[TEXTURE_INDEX_SIZE]; ++j){ ifs.get(); }

		ifs.get();

		const byte shareToonFlag = ifs.get();

		if(shareToonFlag){
			ifs.get();
		} else{
			ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].toonTextureIndex), hederData[TEXTURE_INDEX_SIZE]);
		}

		ifs.read(reinterpret_cast<char*>(&arrayLength), 4);

		for(unsigned int j = 0; j < arrayLength; ++j){ ifs.get(); }

		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].vertexNum), 4);
	}

	// ボーン ---------------------------------
	int numOfBone = 0;
	ifs.read(reinterpret_cast<char*>(&numOfBone), 4);

	boneSize_ = numOfBone;

	pmxData_.bones.resize(numOfBone);

	int ikLinkSize = 0;
	unsigned char angleLim = 0;

	for(int i = 0; i < numOfBone; ++i){
		wstring buf;
		GetPMXStringUTF16(ifs, buf);
		pmxData_.bones[i].name = WStringToString(buf);

		ifs.read(reinterpret_cast<char*>(&arrayLength), 4);
		pmxData_.bones[i].nameEnglish.resize(arrayLength);

		for(unsigned int j = 0; j < arrayLength; ++j){
			pmxData_.bones[i].nameEnglish[j] = ifs.get();
		}

		ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].position), 12);

		ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].parentIndex), hederData[BONE_INDEX_SIZE]);

		if(numOfBone <= pmxData_.bones[i].parentIndex){
			pmxData_.bones[i].parentIndex = PMXModelData::NO_DATA_FLAG;
		}

		ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].transformationLevel), 4);

		ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].flag), 2);

		enum BoneFlagMask{
			ACCESS_POINT = 0x0001,
			IK = 0x0020,
			IMPART_TRANSLATION = 0x0100,
			IMPART_ROTATION = 0x0200,
			AXIS_FIXING = 0x0400,
			LOCAL_AXIS = 0x0800,
			EXTERNAL_PARENT_TRANS = 0x2000,
		};

		if(pmxData_.bones[i].flag & ACCESS_POINT){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].childrenIndex), hederData[BONE_INDEX_SIZE]);
			if(numOfBone <= pmxData_.bones[i].childrenIndex)
			{
				pmxData_.bones[i].childrenIndex = PMXModelData::NO_DATA_FLAG;
			}
		} else{
			pmxData_.bones[i].childrenIndex = PMXModelData::NO_DATA_FLAG;
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].coordOffset), 12);
		}

		if((pmxData_.bones[i].flag & IMPART_TRANSLATION) || (pmxData_.bones[i].flag & IMPART_ROTATION)){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].impartParentIndex), hederData[BONE_INDEX_SIZE]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].impartRate), 4);
		}

		if(pmxData_.bones[i].flag & AXIS_FIXING){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].fixedAxis), 12);
		}

		if(pmxData_.bones[i].flag & LOCAL_AXIS){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].localAxisX), 12);
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].localAxisZ), 12);
		}

		if(pmxData_.bones[i].flag & EXTERNAL_PARENT_TRANS){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].externalParentKey), 4);
		}

		if(pmxData_.bones[i].flag & IK){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikTargetIndex), hederData[5]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLoopCount), 4);
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikUnitAngle), 4);
			ifs.read(reinterpret_cast<char*>(&ikLinkSize), 4);

			pmxData_.bones[i].ikLinks.resize(ikLinkSize);

			for(int j = 0; j < ikLinkSize; ++j){
				ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLinks[j].index), hederData[5]);
				angleLim = ifs.get();

				pmxData_.bones[i].ikLinks[j].existAngleLimited = false;

				if(angleLim == 1){
					ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLinks[j].limitAngleMin), 12);
					ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLinks[j].limitAngleMax), 12);
					pmxData_.bones[i].ikLinks[j].existAngleLimited = true;
				}
			}
		} else{
			pmxData_.bones[i].ikTargetIndex = PMXModelData::NO_DATA_FLAG;
		}
	}

	ifs.close();

	// 頂点情報格納
	vecVertex_.resize(vertexSize_);

	for(unsigned int i = 0; i < vertexSize_; ++i){
		vecVertex_[i].pos.x = pmxData_.vertices[i].position.x;
		vecVertex_[i].pos.y = pmxData_.vertices[i].position.y;
		vecVertex_[i].pos.z = pmxData_.vertices[i].position.z;

		vecVertex_[i].nor.x = pmxData_.vertices[i].normal.x;
		vecVertex_[i].nor.y = pmxData_.vertices[i].normal.y;
		vecVertex_[i].nor.z = pmxData_.vertices[i].normal.z;

		vecVertex_[i].tex.x = pmxData_.vertices[i].uv.x;
		vecVertex_[i].tex.y = pmxData_.vertices[i].uv.y;

		vecVertex_[i].col = ideaColor::WHITE;

		vecVertex_[i].boneIndex[0] = pmxData_.vertices[i].weight.born1;
		vecVertex_[i].boneIndex[1] = pmxData_.vertices[i].weight.born2;
		vecVertex_[i].boneIndex[2] = pmxData_.vertices[i].weight.born3;
		vecVertex_[i].boneIndex[3] = pmxData_.vertices[i].weight.born4;

		vecVertex_[i].weight[0] = pmxData_.vertices[i].weight.weight1;
		vecVertex_[i].weight[1] = pmxData_.vertices[i].weight.weight2;
		vecVertex_[i].weight[2] = pmxData_.vertices[i].weight.weight3;
	}

	// インデックス情報格納
	vecIndex_.resize(indexSize_);

	for(unsigned int i = 0; i < indexSize_; ++i){
		vecIndex_[i] = pmxData_.surfaces[i].vertexIndex;
	}

	// マテリアル情報格納
	vecMaterial_.resize(materialSize_);

	for(unsigned int i = 0; i < materialSize_; ++i){
		vecMaterial_[i].colorMapTextureIndex = pmxData_.materials[i].colorMapTextureIndex;
		vecMaterial_[i].vertexNum = pmxData_.materials[i].vertexNum;
	}

	// ボーン情報格納
	vector<string> vecBoneName(boneSize_);

	for(unsigned int i = 0; i < boneSize_; ++i){
		auto& pd = pmxData_.bones[i];
		vecBoneName[i] = pd.name;
		auto& node = mapBone_[pd.name];
		node.index = i;
		node.pos = pd.position;
	}

	for(auto& pd : pmxData_.bones){
		if(pd.parentIndex >= boneSize_ || pd.parentIndex == PMXModelData::NO_DATA_FLAG){
			continue;
		}
		auto parentName = vecBoneName[pd.parentIndex];
		mapBone_[parentName].vecChildrenPtr.emplace_back(&mapBone_[pd.name]);
	}

	vecBoneMatrix_.resize(boneSize_);

	fill(vecBoneMatrix_.begin(), vecBoneMatrix_.end(), Matrix4x4::Identity());

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

bool PmxModel::LoadPmxMeshFromStorage(const char* pFileName)
{
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// 既に読み込み済みなら終了

	// ストレージから読み込む
	PmxModel& pxm = eidosStorageManager::Instance().GetPmxModel(pFileName);

	// 頂点
	pVertexBuffer_ = pxm.pVertexBuffer_;

	// インデックス
	pIndexBuffer_ = pxm.pIndexBuffer_;

	// pmxデータ
	PMXModelData& pmxData = pxm.pmxData_;
	vertexSize_ = pxm.vertexSize_;
	indexSize_ = pxm.indexSize_;
	materialSize_ = pxm.materialSize_;
	boneSize_ = pxm.boneSize_;

	// 頂点情報格納
	vecVertex_.resize(vertexSize_);

	for(unsigned int i = 0; i < vertexSize_; ++i){
		vecVertex_[i].pos.x = pmxData.vertices[i].position.x;
		vecVertex_[i].pos.y = pmxData.vertices[i].position.y;
		vecVertex_[i].pos.z = pmxData.vertices[i].position.z;

		vecVertex_[i].nor.x = pmxData.vertices[i].normal.x;
		vecVertex_[i].nor.y = pmxData.vertices[i].normal.y;
		vecVertex_[i].nor.z = pmxData.vertices[i].normal.z;

		vecVertex_[i].tex.x = pmxData.vertices[i].uv.x;
		vecVertex_[i].tex.y = pmxData.vertices[i].uv.y;

		vecVertex_[i].col = ideaColor::WHITE;

		vecVertex_[i].boneIndex[0] = pmxData.vertices[i].weight.born1;
		vecVertex_[i].boneIndex[1] = pmxData.vertices[i].weight.born2;
		vecVertex_[i].boneIndex[2] = pmxData.vertices[i].weight.born3;
		vecVertex_[i].boneIndex[3] = pmxData.vertices[i].weight.born4;

		vecVertex_[i].weight[0] = pmxData.vertices[i].weight.weight1;
		vecVertex_[i].weight[1] = pmxData.vertices[i].weight.weight2;
		vecVertex_[i].weight[2] = pmxData.vertices[i].weight.weight3;
	}

	// インデックス情報格納
	vecIndex_.resize(indexSize_);

	for(unsigned int i = 0; i < indexSize_; ++i){
		vecIndex_[i] = pmxData.surfaces[i].vertexIndex;
	}

	// マテリアル情報格納
	vecMaterial_.resize(materialSize_);

	for(unsigned int i = 0; i < materialSize_; ++i){
		vecMaterial_[i].colorMapTextureIndex = pmxData.materials[i].colorMapTextureIndex;
		vecMaterial_[i].vertexNum = pmxData.materials[i].vertexNum;
	}

	// ボーン情報格納
	vector<string> vecBoneName(boneSize_);

	for(unsigned int i = 0; i < boneSize_; ++i){
		auto& pd = pmxData.bones[i];
		vecBoneName[i] = pd.name;
		auto& node = mapBone_[pd.name];
		node.index = i;
		node.pos = pd.position;
	}

	for(auto& pd : pmxData.bones){
		if(pd.parentIndex >= boneSize_ || pd.parentIndex == PMXModelData::NO_DATA_FLAG){
			continue;
		}
		auto parentName = vecBoneName[pd.parentIndex];
		mapBone_[parentName].vecChildrenPtr.emplace_back(&mapBone_[pd.name]);
	}

	vecBoneMatrix_.resize(boneSize_);

	fill(vecBoneMatrix_.begin(), vecBoneMatrix_.end(), Matrix4x4::Identity());

	// テクスチャ
	texPtrSize_ = pxm.texPtrSize_;
	copy(pxm.vecTexPtr_.begin(), pxm.vecTexPtr_.end(), back_inserter(vecTexPtr_));


	if(pVertexBuffer_ && pIndexBuffer_){ bStorage_ = true; }	// ストレージ使用フラグをオンに

	return true;
}

void PmxModel::UnLoad()
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
	vector<unsigned short>().swap(vecIndex_);

	boneSize_ = 0;

	vector<PMXModelData::Vertex>().swap(pmxData_.vertices);
	vector<PMXModelData::Surface>().swap(pmxData_.surfaces);
	vector<string>().swap(pmxData_.texturePaths);
	vector<PMXModelData::Material>().swap(pmxData_.materials);
	vector<PMXModelData::Bone>().swap(pmxData_.bones);

	map<string, VmdBone>().swap(mapBone_);

	vector<Matrix4x4>().swap(vecBoneMatrix_);

	materialSize_ = 0;

	texPtrSize_ = 0;
	vector<Texture*>().swap(vecTexPtr_);
}

void PmxModel::Draw(Camera* pCamera)
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

	//定数バッファ(vmd)
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

		if(texPtrSize_ > 0 && vecMaterial_[0].colorMapTextureIndex != 255){
			pTexView = vecTexPtr_[vecMaterial_[0].colorMapTextureIndex]->GetTextureViewPtr();
		}

		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
		} else{
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
		}

		gm.GetContextPtr()->DrawIndexed(indexSize_, 0, 0);
	} else{
		unsigned long strat = 0;

		for(unsigned int i = 0; i < materialSize_; ++i){
			ID3D11ShaderResourceView* pTexView = nullptr;

			if(texPtrSize_ > 0 && vecMaterial_[i].colorMapTextureIndex != 255){
				pTexView = vecTexPtr_[vecMaterial_[i].colorMapTextureIndex]->GetTextureViewPtr();
			}

			if(pTexView){
				gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
			} else{
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
			}

			gm.GetContextPtr()->DrawIndexed(vecMaterial_[i].vertexNum, strat, 0);

			strat += vecMaterial_[i].vertexNum;
		}
	}
}