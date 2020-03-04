#ifndef INCLUDE_EIDOS_FBXMESH_H
#define INCLUDE_EIDOS_FBXMESH_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include <fbxsdk.h>
#include <vector>
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../eidos/h/3D/Object.h"
#include "../../../eidos/h/3D/ObjectManager.h"
#include "../../../eidos//h/Mesh/ObjModel.h"

class FbxMeshHolder;
//class FbxManager;
//class FbxScene;
//class FbxMesh;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;
//
//マテリアル構造体
struct Material
{
	char* pMaterialName;
	Color color;
	const char* pFileName;
	Texture* pTexture;
};
//
//struct ObjSubset
//{
//	unsigned int materialIndex;
//	unsigned int faceStart;
//	unsigned int faceCount;
//};
//頂点構造体
struct FbxMeshData
{
	char* pTextureFileName;
	Texture* pTexture;
	unsigned int nNumPolygon;
	unsigned int nNumPolygonVertex;
	unsigned int nNumVertex;
	unsigned int nNumUv;
	unsigned int nNumIndexUv;
	VertexData3D* pVd;
	Vector2D* pTexIndex;
	unsigned short* pIndexNumber;
	int* pUvIndexNumber;
	Color color;
};

class FbxModel : public Object{
public:
	FbxModel();
	~FbxModel();
	bool LoadFbxMeshFromFile(const char* pFileName);//読み込み
	bool LoadFbxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//読み込み
	//bool LoadFbxMeshFromStorage(const char* pFileName);	// ストレージから読み込む
	void UnLoad();
	ID3D11Buffer* GetVertexBufferPtr()const{ return pVertexBuffer_; }
	ID3D11Buffer* GetIndexBufferPtr()const{ return pIndexBuffer_; }
	unsigned int GetVertexSize()const{ return vertexSize_; };
	ObjSubset* GetSubsetPtr(){ return &vecSubset_[0]; }
	unsigned int GetSubsetSize()const{ return subsetSize_; };
	Texture** GetTexturePtrPtr(){ return &vecTexPtr_[0]; }
	unsigned int GetTexturePtrSize()const{ return texPtrSize_; };

private:
	FbxManager* pFbxManager_;
	FbxScene* pFbxScene_;
	FbxMesh* pFbxMesh_;
	ID3D11Buffer* pVertexBuffer_;//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;//インデックスバッファ
	std::vector<ID3D11Buffer*> vecVertexBufferPtr_;
	std::vector<ID3D11Buffer*> vecIndexBufferPtr_;

	std::vector<MeshVertexData> vecVertex_;
	FbxMeshData* m_pMesh;
	Material* m_pMaterial;
	std::vector<ObjSubset> vecSubset_;
	std::vector<ObjMaterial> vecMaterial_;
	std::vector<unsigned short> vecIndex_;
	unsigned int vertexSize_;
	unsigned int indexSize_;
	unsigned int subsetSize_;
	unsigned int materialSize_;
	unsigned int texPtrSize_;
	unsigned int m_nNumMesh;
	std::vector<Texture*> vecTexPtr_;
	bool bStorage_;		// ストレージ使用フラグ

	void SetupNode(FbxNode* pNode, std::string parentName, int faceStart, const char * pFileName);

	void Draw(Camera* pCamera)override;//描画
	//bool LoadMaterialFromFile(const char* pFileName);//マテリアルファイル読み込み
	//bool LoadMaterialFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//マテリアルファイル読み込み

																						  // コピーコンストラクタの禁止
	FbxModel(const FbxModel& src){}
	FbxModel& operator=(const FbxModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_FBXMESH_H
