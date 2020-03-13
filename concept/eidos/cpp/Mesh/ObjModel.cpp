#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../../h/Mesh/ObjModel.h"
//#include "../../h/Mesh/ObjMeshHolder.h"
#include "../../h/eidosStorage/eidosStorageManager.h"
#include "../../../idea/h/Framework/GraphicManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Archive/ArchiveLoader.h"
#include "../../../idea/h/Utility/ideaUtility.h"

using namespace std;
using namespace DirectX;


ObjModel::ObjModel() :
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

ObjModel::~ObjModel()
{
	UnLoad();
}

bool ObjModel::LoadObjMeshFromFile(const char * pFileName)
{
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// 既に読み込み済みなら終了

	GraphicManager& gm = GraphicManager::Instance();

	if(!gm.GetContextPtr()){ return false; }

	char buf[1024] = { 0 };			// ファイルデータバッファ
	char mtlFileName[256] = { 0 };	// マテリアルファイルネーム
	vector<Vector3D> pos;			// 頂点座標
	vector<Vector3D> nor;			// 法線ベクトル
	vector<Vector2D> tex;			// テクスチャ座標
	ObjMaterial material;			// マテリアル情報
	unsigned int dwFaceIndex = 0;	// 面のインデックス
	unsigned int dwFaceCount = 0;	// 面のカウント
	unsigned int dwCurSubset = 0;	// サブセットの位置
	int preSize = 0;				// 事前のサブセットのサイズ

	// ファイルを開く
	ifstream file;
	file.open(pFileName, ios::in);
	if(!file.is_open()){ return false; }

	// objファイル解析
	for(;;)
	{
		file >> buf;
		if(!file){ break; }


		if(strcmp(buf, "#") == 0){	// コメント
		} else if(strcmp(buf, "v") == 0){	// 頂点座標
			float x, y, z;
			file >> x >> y >> z;
			Vector3D v(x, y, z);
			pos.push_back(Vector3D(x, y, z));
		} else if(strcmp(buf, "vt") == 0){	// テクスチャ座標
			float u, v;
			file >> u >> v;
			tex.push_back(Vector2D(u, v));
		} else if(strcmp(buf, "vn") == 0){	// 法線ベクトル
			float x, y, z;
			file >> x >> y >> z;
			nor.push_back(Vector3D(x, y, z));
		} else if(strcmp(buf, "f") == 0){	// 面
			unsigned int iPosition, iTexCoord, iNormal;
			unsigned int p[4] = { (unsigned int)-1 }, t[4] = { (unsigned int)-1 }, n[4] = { (unsigned int)-1 };
			MeshVertexData vertex;
			dwFaceIndex++;
			dwFaceCount++;
			int count = 0;
			unsigned int index = 0;

			// 三角形・四角形のみ対応
			for(int iFace = 0; iFace < 4; ++iFace){
				ZeroMemory(&vertex, sizeof(vertex));
				++count;	// 頂点数を数える

				// 頂点座標インデックス
				file >> iPosition;
				vertex.pos = pos[iPosition - 1];
				p[iFace] = iPosition - 1;

				if(file.peek() == '/'){
					file.ignore();

					// テクスチャ座標インデックス
					if(file.peek() != '/'){
						file >> iTexCoord;
						if(iTexCoord){
							vertex.tex = tex[iTexCoord - 1];
							t[iFace] = iTexCoord - 1;
						} else{
							vertex.tex = Vector2D(0.0f, 0.0f);
							t[iFace] = 0;
						}
					}

					// 法線ベクトルインデックス
					if(file.peek() == '/'){
						file.ignore();

						file >> iNormal;
						vertex.nor = nor[iNormal - 1];
						n[iFace] = iNormal - 1;
					}
				}

				// カウントが3未満
				if(iFace < 3){
					vecVertex_.push_back(vertex);
					index = vecVertex_.size() - 1;
					vecIndex_.push_back(index);
				}

				// 次が改行だったら終了
				if(file.peek() == '\n'){
					break;
				}

			}

			// 四角形ポリゴンの場合，三角形を追加する
			if(count > 3){
				//　カウント
				++dwFaceIndex;
				++dwFaceCount;

				//　頂点とインデックスを追加
				for(int iFace = 1; iFace < 4; ++iFace){
					ZeroMemory(&vertex, sizeof(vertex));

					int j = (iFace + 1) % 4;

					if(p[j] != -1) vertex.pos = pos[p[j]];
					if(t[j] != -1) vertex.tex = tex[t[j]];
					if(n[j] != -1) vertex.nor = nor[n[j]];
					vecVertex_.push_back(vertex);

					index = vecVertex_.size() - 1;
					vecIndex_.push_back(index);
				}

			}
		} else if(strcmp(buf, "mtllib") == 0){	// マテリアルファイル
			file >> mtlFileName;
			string t = mtlFileName;
			string s = pFileName;
			AddDirectoryPath(t, s);

			//　マテリアルファイルの読み込み
			if(mtlFileName[0]){
				if(!LoadMaterialFromFile(t.c_str())){ return false; }
			}
		} else if(strcmp(buf, "usemtl") == 0){	// マテリアル
			char strName[256] = {};
			file >> strName;

			ObjSubset subset;
			subset.faceCount = 1;

			for(unsigned int i = 0; i < materialSize_; ++i){
				if(strcmp(vecMaterial_[i].name, strName) == 0){
					dwCurSubset = i;
					break;
				}
			}

			subset.materialIndex = dwCurSubset;
			subset.faceStart = dwFaceIndex * 3;
			preSize = vecSubset_.size();
			vecSubset_.push_back(subset);

			if(vecSubset_.size() > 1){
				vecSubset_[preSize - 1].faceCount = dwFaceCount * 3;
				dwFaceCount = 0;
			}
		}

		file.ignore(1024, '\n');
	}

	file.close();

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


	return true;
}

bool ObjModel::LoadObjMeshFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// 既に読み込み済みなら終了

	GraphicManager& gm = GraphicManager::Instance();

	if(!gm.GetContextPtr()){ return false; }

	// tmpファイル名作成
	char tmpP[] = "p";
	char tempFileData[] = "obj_";
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

	char buf[1024] = {};			// ファイルデータバッファ
	char mtlFileName[256] = {};		// マテリアルファイルネーム
	vector<Vector3D> pos;			// 頂点座標
	vector<Vector3D> nor;			// 法線ベクトル
	vector<Vector2D> tex;			// テクスチャ座標
	ObjMaterial material;			// マテリアル情報
	unsigned int dwFaceIndex = 0;	// 面のインデックス
	unsigned int dwFaceCount = 0;	// 面のカウント
	unsigned int dwCurSubset = 0;	// サブセットの位置
	int preSize = 0;				// 事前のサブセットのサイズ

	// objファイル解析
	for(;;)
	{
		tmpStream >> buf;
		if(!tmpStream){ break; }


		if(strcmp(buf, "#") == 0){	// コメント
		} else if(strcmp(buf, "v") == 0){	// 頂点座標
			float x, y, z;
			tmpStream >> x >> y >> z;
			Vector3D v(x, y, z);
			pos.push_back(Vector3D(x, y, z));
		} else if(strcmp(buf, "vt") == 0){	// テクスチャ座標
			float u, v;
			tmpStream >> u >> v;
			tex.push_back(Vector2D(u, v));
		} else if(strcmp(buf, "vn") == 0){	// 法線ベクトル
			float x, y, z;
			tmpStream >> x >> y >> z;
			nor.push_back(Vector3D(x, y, z));
		} else if(strcmp(buf, "f") == 0){	// 面
			unsigned int iPosition, iTexCoord, iNormal;
			unsigned int p[4] = {}, t[4] = {}, n[4] = {};
			MeshVertexData vertex;
			dwFaceIndex++;
			dwFaceCount++;
			int count = 0;
			unsigned int index = 0;

			// 三角形・四角形のみ対応
			for(int iFace = 0; iFace < 4; ++iFace){
				ZeroMemory(&vertex, sizeof(vertex));
				++count;	// 頂点数を数える

				// 頂点座標インデックス
				tmpStream >> iPosition;
				vertex.pos = pos[iPosition - 1];
				p[iFace] = iPosition - 1;

				if(tmpStream.peek() == '/'){
					tmpStream.ignore();

					// テクスチャ座標インデックス
					if(tmpStream.peek() != '/'){
						tmpStream >> iTexCoord;
						if(iTexCoord){
							vertex.tex = tex[iTexCoord - 1];
							t[iFace] = iTexCoord - 1;
						} else{
							vertex.tex = Vector2D(0.0f, 0.0f);
							t[iFace] = 0;
						}
					}

					// 法線ベクトルインデックス
					if(tmpStream.peek() == '/'){
						tmpStream.ignore();

						tmpStream >> iNormal;
						vertex.nor = nor[iNormal - 1];
						n[iFace] = iNormal - 1;
					}
				}

				// カウントが3未満
				if(iFace < 3){
					vecVertex_.push_back(vertex);
					index = vecVertex_.size() - 1;
					vecIndex_.push_back(index);
				}

				// 次が改行だったら終了
				if(tmpStream.peek() == '\n'){
					break;
				}

			}

			// 四角形ポリゴンの場合，三角形を追加する
			if(count > 3){
				// カウント
				++dwFaceIndex;
				++dwFaceCount;

				// 頂点とインデックスを追加
				for(int iFace = 1; iFace < 4; ++iFace){
					ZeroMemory(&vertex, sizeof(vertex));

					int j = (iFace + 1) % 4;

					if(p[j] != -1) vertex.pos = pos[p[j]];
					if(t[j] != -1) vertex.tex = tex[t[j]];
					if(n[j] != -1) vertex.nor = nor[n[j]];
					vecVertex_.push_back(vertex);

					index = vecVertex_.size() - 1;
					vecIndex_.push_back(index);
				}

			}
		} else if(strcmp(buf, "mtllib") == 0){	// マテリアルファイル
			tmpStream >> mtlFileName;
			ArchiveLoader loader;
			if(!loader.Load(pArchiveFileName, mtlFileName)){ return false; }

			// tmpファイル名作成
			char tempFileData[] = "mtl_";
			char tempFileNum[] = "000";
			char tempFileName_[256] = "eidos_";
			strcat_s(tempFileName_, tempFileData);
			strcat_s(tempFileName_, tempFileNum);
			strcat_s(tempFileName_, tmpDot);
			strcat_s(tempFileName_, tmpT);
			strcat_s(tempFileName_, tmpM);
			strcat_s(tempFileName_, tmpP);

			// tmpファイル書き込み
			FILE* fp;
			if(fopen_s(&fp, tempFileName_, "wb") != 0){ return false; }

			if(fp == NULL) {
				return false;
			} else {
				fwrite(&loader.GetData()[0], loader.GetSize(), sizeof(BYTE), fp);
				fclose(fp);
			}

			//　マテリアルファイルの読み込み
			if(mtlFileName[0]){
				if(!LoadMaterialFromArchiveFile(pArchiveFileName, tempFileName_)){ return false; }
			}

			if(fopen_s(&fp, tempFileName_, "wb") != 0){ return false; }

			if(fp == NULL) {
				return false;
			} else {
				fwrite("", 1, sizeof(BYTE), fp);
			}
			fclose(fp);

			DeleteFileA(tempFileName_);
		} else if(strcmp(buf, "usemtl") == 0){	// マテリアル
			char strName[256] = { 0 };
			tmpStream >> strName;

			ObjSubset subset;
			subset.faceCount = 1;

			for(unsigned int i = 0; i < materialSize_; ++i){
				if(strcmp(vecMaterial_[i].name, strName) == 0){
					dwCurSubset = i;
					break;
				}
			}

			subset.materialIndex = dwCurSubset;
			subset.faceStart = dwFaceIndex * 3;
			preSize = vecSubset_.size();
			vecSubset_.push_back(subset);

			if(vecSubset_.size() > 1){
				vecSubset_[preSize - 1].faceCount = dwFaceCount * 3;
				dwFaceCount = 0;
			}
		}

		tmpStream.ignore(1024, '\n');
	}

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

	return true;
}

bool ObjModel::LoadObjMeshFromStorage(const char * pFileName)
{
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// 既に読み込み済みなら終了

	// ストレージから読み込む
	// 頂点
	vertexSize_ = eidosStorageManager::Instance().GetObjModel(pFileName).vertexSize_;
	pVertexBuffer_ = eidosStorageManager::Instance().GetObjModel(pFileName).pVertexBuffer_;
	copy(eidosStorageManager::Instance().GetObjModel(pFileName).vecVertex_.begin(), eidosStorageManager::Instance().GetObjModel(pFileName).vecVertex_.end(), back_inserter(vecVertex_));

	// インデックス
	indexSize_ = eidosStorageManager::Instance().GetObjModel(pFileName).indexSize_;
	pIndexBuffer_ = eidosStorageManager::Instance().GetObjModel(pFileName).pIndexBuffer_;
	copy(eidosStorageManager::Instance().GetObjModel(pFileName).vecIndex_.begin(), eidosStorageManager::Instance().GetObjModel(pFileName).vecIndex_.end(), back_inserter(vecIndex_));

	// サブセット
	subsetSize_ = eidosStorageManager::Instance().GetObjModel(pFileName).subsetSize_;
	copy(eidosStorageManager::Instance().GetObjModel(pFileName).vecSubset_.begin(), eidosStorageManager::Instance().GetObjModel(pFileName).vecSubset_.end(), back_inserter(vecSubset_));

	// マテリアル
	materialSize_ = eidosStorageManager::Instance().GetObjModel(pFileName).materialSize_;
	copy(eidosStorageManager::Instance().GetObjModel(pFileName).vecMaterial_.begin(), eidosStorageManager::Instance().GetObjModel(pFileName).vecMaterial_.end(), back_inserter(vecMaterial_));

	// テクスチャ
	texPtrSize_ = eidosStorageManager::Instance().GetObjModel(pFileName).texPtrSize_;
	copy(eidosStorageManager::Instance().GetObjModel(pFileName).vecTexPtr_.begin(), eidosStorageManager::Instance().GetObjModel(pFileName).vecTexPtr_.end(), back_inserter(vecTexPtr_));

	if(pVertexBuffer_ && pIndexBuffer_){ bStorage_ = true; }	// ストレージ使用フラグをオンに

	return true;
}

void ObjModel::UnLoad()
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
	vector<MeshVertexData>().swap(vecVertex_);

	indexSize_ = 0;
	vector<WORD>().swap(vecIndex_);

	subsetSize_ = 0;
	vector<ObjSubset>().swap(vecSubset_);

	materialSize_ = 0;
	vector<ObjMaterial>().swap(vecMaterial_);

	texPtrSize_ = 0;
	vector<Texture*>().swap(vecTexPtr_);
}

bool ObjModel::LoadMaterialFromFile(const char * pFileName)
{
	char buf[1024] = {};
	int iMtlCount = -1;
	ifstream file;
	ObjMaterial material;
	ZeroMemory(&material, sizeof(ObjMaterial));
	material.ambient = Vector3D(0.2f, 0.2f, 0.2f);
	material.diffuse = Vector3D(0.8f, 0.8f, 0.8f);
	material.specular = Vector3D(1.0f, 1.0f, 1.0f);
	material.shininess = 0.0f;
	material.alpha = 1.0f;

	//　ファイルを開く
	file.open(pFileName, ios::in);
	if(!file.is_open()){ return false; }

	// mtlファイル解析
	for(;;)
	{
		file >> buf;
		if(!file){ break; }

		if(strcmp(buf, "newmtl") == 0){
			iMtlCount++;
			vecMaterial_.push_back(material);
			char strName[256] = {};
			file >> strName;
			strcpy_s(vecMaterial_[iMtlCount].name, strName);
			Texture* pTex = new Texture;
			vecTexPtr_.push_back(pTex);
		} else if(strcmp(buf, "Ka") == 0){
			float r, g, b;
			file >> r >> g >> b;
			vecMaterial_[iMtlCount].ambient = Vector3D(r, g, b);
		} else if(strcmp(buf, "Kd") == 0){
			float r, g, b;
			file >> r >> g >> b;
			vecMaterial_[iMtlCount].diffuse = Vector3D(r, g, b);
		} else if(strcmp(buf, "Ks") == 0){
			float r, g, b;
			file >> r >> g >> b;
			vecMaterial_[iMtlCount].specular = Vector3D(r, g, b);
		} else if(strcmp(buf, "d") == 0 || strcmp(buf, "Tr") == 0){
			file >> vecMaterial_[iMtlCount].alpha;
		} else if(strcmp(buf, "Ns") == 0){
			file >> vecMaterial_[iMtlCount].shininess;
		} else if(strcmp(buf, "map_Ka") == 0){
		} else if(strcmp(buf, "map_Kd") == 0){
			char mapKdName[256] = {};
			file >> mapKdName;
			string t = mapKdName;
			string s = pFileName;
			AddDirectoryPath(t, s);
			vecTexPtr_[vecMaterial_.size() - 1]->LoadImageFromFile(t.c_str());
		} else if(strcmp(buf, "map_Ks") == 0){
		} else if(strcmp(buf, "map_Bump") == 0){
		}

		file.ignore(1024, '\n');
	}

	file.close();

	// マテリアルデータをコピー
	materialSize_ = vecMaterial_.size();

	return true;
}

bool ObjModel::LoadMaterialFromArchiveFile(const char* pArchiveFileName, const char * pFileName)
{
	char buf[1024] = { 0 };
	int iMtlCount = -1;
	ObjMaterial material;
	ZeroMemory(&material, sizeof(ObjMaterial));
	material.ambient = Vector3D(0.2f, 0.2f, 0.2f);
	material.diffuse = Vector3D(0.8f, 0.8f, 0.8f);
	material.specular = Vector3D(1.0f, 1.0f, 1.0f);
	material.shininess = 0.0f;
	material.alpha = 1.0f;

	char tmpP[] = "p";
	char tempFileData[] = "mtl_";
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

	// ファイルを開く
	stringstream tmpStream;
	ifstream file;
	file.open(pFileName, ios::in);
	if(!file.is_open()){ return false; }
	tmpStream << file.rdbuf();

	file.close();

	// mtlファイル解析
	for(;;)
	{
		tmpStream >> buf;
		if(!tmpStream){ break; }

		if(strcmp(buf, "newmtl") == 0){
			iMtlCount++;
			vecMaterial_.push_back(material);
			char strName[256] = {};
			file >> strName;
			strcpy_s(vecMaterial_[iMtlCount].name, strName);
			Texture* pTex = new Texture;
			vecTexPtr_.push_back(pTex);
		} else if(strcmp(buf, "Ka") == 0){
			float r, g, b;
			file >> r >> g >> b;
			vecMaterial_[iMtlCount].ambient = Vector3D(r, g, b);
		} else if(strcmp(buf, "Kd") == 0){
			float r, g, b;
			file >> r >> g >> b;
			vecMaterial_[iMtlCount].diffuse = Vector3D(r, g, b);
		} else if(strcmp(buf, "Ks") == 0){
			float r, g, b;
			file >> r >> g >> b;
			vecMaterial_[iMtlCount].specular = Vector3D(r, g, b);
		} else if(strcmp(buf, "d") == 0 || strcmp(buf, "Tr") == 0){
			file >> vecMaterial_[iMtlCount].alpha;
		} else if(strcmp(buf, "Ns") == 0){
			file >> vecMaterial_[iMtlCount].shininess;
		} else if(strcmp(buf, "map_Ka") == 0){
		} else if(strcmp(buf, "map_Kd") == 0){
			char mapKdName[256] = {};
			tmpStream >> mapKdName;
			vecTexPtr_[vecMaterial_.size() - 1]->LoadImageFromArchiveFile(pArchiveFileName, mapKdName);
		} else if(strcmp(buf, "map_Ks") == 0){
		} else if(strcmp(buf, "map_Bump") == 0){
		}

		tmpStream.ignore(1024, '\n');
	}

	//　マテリアルデータをコピー
	materialSize_ = vecMaterial_.size();

	return true;
}

void ObjModel::Draw(Camera * pCamera)
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
	ID3D11Buffer* pVBuf = pVertexBuffer_;
	UINT stride = sizeof(MeshVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	//インデックスバッファをセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

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

	ID3D11ShaderResourceView* const pSRV[1] = { NULL };
	gm.GetContextPtr()->PSSetShaderResources(0, 1, pSRV);

	// サブセット毎に描画
	if(subsetSize_ <= 1){
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

		gm.GetContextPtr()->Draw(vertexSize_, 0);
	} else{
		for(unsigned int i = 1; i < subsetSize_; ++i){
			ID3D11ShaderResourceView* pTexView = nullptr;
			if(texPtrSize_ > i){
				pTexView = vecTexPtr_[i - 1]->GetTextureViewPtr();
			}

			if(pTexView){
				gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
			} else{
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
			}

			gm.GetContextPtr()->Draw(vecSubset_[i].faceStart - vecSubset_[i - 1].faceStart, vecSubset_[i - 1].faceStart);///
		}

		ID3D11ShaderResourceView* pTexView = nullptr;
		if(texPtrSize_ == subsetSize_ - 1){
			pTexView = vecTexPtr_[subsetSize_ - 1]->GetTextureViewPtr();
		}

		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
		} else{
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
		}

		gm.GetContextPtr()->Draw(vertexSize_ - vecSubset_[subsetSize_ - 1].faceStart, vecSubset_[subsetSize_ - 1].faceStart);///
	}
}
