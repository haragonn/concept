#ifndef INCLUDE_EIDOS_PMDMODEL_H
#define INCLUDE_EIDOS_PMDMODEL_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../../h/Utility/eidosType.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"

#include <vector>
#include <map>
#include <string>

struct ID3D11Buffer;

class PmdModel : public Object{
public:
	PmdModel();
	~PmdModel();

	bool LoadPmdMeshFromFile(const char* pFileName);//読み込み
	bool LoadPmdMeshFromStorage(const char* pFileName);	// ストレージから読み込む

	void UnLoad();

private:
	friend class VmdMotion;

	ID3D11Buffer* pVertexBuffer_;//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;//インデックスバッファ

	std::vector<BlendVertexData> vecVertex_;
	unsigned long vertexSize_;
	std::vector<unsigned short> vecIndex_;
	unsigned long indexSize_;
	std::vector<PmdMaterial> vecMaterial_;
	unsigned long materialSize_;
	std::vector<PmdBone> vecPmdBone_;
	std::map<std::string, VmdBone> mapBone_;
	std::vector<Matrix4x4> vecBoneMatrix_;
	unsigned short boneSize_;
	std::vector<Texture*> vecTexPtr_;
	unsigned long texPtrSize_;

	bool bStorage_;		// ストレージ使用フラグ

	void Draw(Camera* pCamera)override;//描画

	// コピーコンストラクタの禁止
	PmdModel(const PmdModel& src){}
	PmdModel& operator=(const PmdModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_PMDMODEL_H