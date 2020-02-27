#ifndef INCLUDE_EIDOS_PMDMODEL_H
#define INCLUDE_EIDOS_PMDMODEL_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"

#include <vector>
#include <map>
#include <string>

struct ID3D11Buffer;

// ブレンド重み付メッシュの頂点データ
struct BlendVertexData
{
	Vector3D pos;
	Vector3D nor;
	Color col;
	Vector2D tex;
	float weight[3];
	unsigned long boneIndex[4];
};

#pragma pack(push,1)
// PMDファイルのヘッダー
struct PmdHeader
{
	unsigned char magic[3];			//"Pmd"
	float version;					//00 00 80 3F
	unsigned char modelName[20];	//モデル名
	unsigned char comment[256];		//モデル付属コメント
};

// PMDファイルから読み込む頂点情報
struct PmdVertex
{
	Vector3D pos;
	Vector3D nor;
	Vector2D tex;
	unsigned short boneNum[2];		//影響ボーン番号
	unsigned char boneWeight;		//影響度
	unsigned char edgeFlag;			//エッジ
};

// PMDファイルから読み込むマテリアル情報
struct PmdMaterial
{
	float diffuse[3];			//減衰色
	float alpha;				//透明率
	float specularity;			//光沢レベル
	float specular[3];			//光沢色
	float ambient[3];			//環境色
	unsigned char toonIndex;	//toon
	unsigned char edgeFlag;		//輪郭
	unsigned long faceCount;	// この材料の面頂点数
	char textureFileName[20];	//テクスチャファイルネーム
};

// PDMファイルから読み込むボーン情報
struct PmdBone 
{
	char boneName[20];					//ボーン名
	unsigned short parentBoneIndex	;	//親ボーン番号 (ない場合は0xFFFF
	unsigned short tailPosBoneIndex;	//tail位置のボーン番号 (ない場合は0xFFFF
	unsigned char boneType;				//ボーンの種類 0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 5:回転影響下 6:IK接続先 7:非表示 8:捻り 9:回転運動 (8, 9はMMD4.0以降)
	unsigned short ikParentBoneIndex;	//IKボーン番号 (ない場合は0
	Vector3D boneHeadPos;				//ボーンのヘッド位置
};

#pragma pack(pop)

// ボーン
struct Bone 
{
	Bone() : type(0), index(-1)
	{
		std::vector<Bone*>().swap(vecChildrenPtr);
	};

	long index;							// ボーンID（通し番号）
	std::string name;				// ボーン名
	unsigned char type;				// ボーンタイプ (MMDの場合 0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 5:回転影響下 6:IK接続先 7:非表示 8:捻り 9:回転運動 )
	Vector3D pos;
	std::vector<Bone*> vecChildrenPtr;
};

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
	std::map<std::string, Bone> mapBone_;
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