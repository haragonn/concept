#ifndef INCLUDE_EIDOS_OBJMODEL_H
#define INCLUDE_EIDOS_OBJMODEL_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../Utility/eidosType.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../idea/h/Utility/ideaUtility.h"
#include <vector>

struct ID3D11ShaderResourceView;
struct ID3D11Buffer;

class ObjModel : public Object{
public:
	ObjModel();
	~ObjModel();

	bool LoadObjMeshFromFile(const char* pFileName);//読み込み
	bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//読み込み
	bool LoadObjMeshFromStorage(const char* pFileName);	// ストレージから読み込む

	void UnLoad();

private:
	ID3D11Buffer* pVertexBuffer_;//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;//インデックスバッファ

	std::vector<MeshVertexData> vecVertex_;
	unsigned int vertexSize_;

	std::vector<unsigned short> vecIndex_;
	unsigned int indexSize_;

	std::vector<ObjSubset> vecSubset_;
	unsigned int subsetSize_;

	std::vector<ObjMaterial> vecMaterial_;
	unsigned int materialSize_;

	std::vector<Texture*> vecTexPtr_;
	unsigned int texPtrSize_;

	bool bStorage_;		// ストレージ使用フラグ

	bool LoadMaterialFromFile(const char* pFileName);//マテリアルファイル読み込み
	bool LoadMaterialFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//マテリアルファイル読み込み

	void Draw(Camera* pCamera)override;

	// コピーコンストラクタの禁止
	ObjModel(const ObjModel& src){}
	ObjModel& operator=(const ObjModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_OBJMODEL_H
