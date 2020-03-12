#ifndef INCLUDE_EIDOS_FBXMESH_H
#define INCLUDE_EIDOS_FBXMESH_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include <fbxsdk.h>
#include <vector>
#include "../../../eidos/h/3D/Object.h"
#include "../../../eidos/h/3D/ObjectManager.h"
#include "../../../eidos/h/Utility/eidosType.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"

struct ID3D11ShaderResourceView;
struct ID3D11Buffer;

class FbxModel : public Object{
public:
	FbxModel();
	~FbxModel();

	bool LoadFbxMeshFromFile(const char* pFileName);//読み込み
	bool LoadFbxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//読み込み
	//bool LoadFbxMeshFromStorage(const char* pFileName);	// ストレージから読み込む

	void UnLoad();

private:
	FbxManager* pFbxManager_;
	FbxScene* pFbxScene_;
	FbxMesh* pFbxMesh_;

	ID3D11Buffer* pVertexBuffer_;//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;//インデックスバッファ

	std::vector<ID3D11Buffer*> vecVertexBufferPtr_;
	std::vector<ID3D11Buffer*> vecIndexBufferPtr_;

	std::vector<MeshVertexData> vecVertex_;
	unsigned int vertexSize_;

	std::vector<unsigned short> vecIndex_;
	unsigned int indexSize_;

	std::vector<FbxMeshData> vecMesh_;
	unsigned int meshSize_;

	std::vector<FbxMaterial> vecMaterial_;
	unsigned int materialSize_;

	std::vector<Texture*> vecTexPtr_;
	unsigned int texPtrSize_;

	bool bStorage_;		// ストレージ使用フラグ

	void SetupNode(FbxNode* pNode, std::string parentName, int faceStart, const char * pFileName);

	void Draw(Camera* pCamera)override;//描画

	// コピーコンストラクタの禁止
	FbxModel(const FbxModel& src){}
	FbxModel& operator=(const FbxModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_FBXMESH_H
