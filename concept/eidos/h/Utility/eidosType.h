#ifndef INCLUDE_EIDOS_EIDOSTYPE_H
#define INCLUDE_EIDOS_EIDOSTYPE_H

#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../idea/h/Utility/ideaColor.h"

#include <vector>
#include <map>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <directxmath.h>

// 頂点構造体
struct MeshVertexData
{
	Vector3D pos;
	Vector3D nor;
	Color color;
	Vector2D tex;
};

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

struct ConstBuffer3D
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 light;
};

struct ConstBuffer3DShadow
{
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 light;
	DirectX::XMFLOAT4X4 lightView;
	DirectX::XMFLOAT4X4 lightProj;
};

struct ConstBufferBoneWorld
{
	DirectX::XMFLOAT4X4 boneWorld[512];
};

// .obj
#pragma pack(push,1)

// サブセット
struct ObjSubset
{
	unsigned int materialIndex;
	unsigned int faceStart;
	unsigned int faceCount;
};

// マテリアル構造体
struct ObjMaterial
{
	char name[256];
	Vector3D ambient;
	Vector3D diffuse;
	Vector3D specular;
	float shininess;
	float alpha;
	char ambientMapName[256];
	char diffuseMapName[256];
	char specularMapName[256];
	char bumpMapName[256];
};

#pragma pack(pop)

// .fbx
//マテリアル構造体
struct FbxMaterial
{
	char* pMaterialName;
	Color color;
	const char* pFileName;
	Texture* pTexture;
};

//頂点構造体
struct FbxMeshData
{
	char* pTextureFileName;
	Texture* pTexture;
	unsigned int nNumPolygon;
	unsigned int nNumIndex;
	unsigned int nNumVertex;
	unsigned int nNumUv;
	unsigned int nNumIndexUv;
	std::vector<MeshVertexData> vecVd;
	std::vector<Vector2D> vecTexIndex;
	std::vector<unsigned short> vecIndexNumber;
	std::vector<int> vecUvIndexNumber;
	Color color;
};

// .pmd
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
	unsigned short parentBoneIndex;	//親ボーン番号 (ない場合は0xFFFF
	unsigned short tailPosBoneIndex;	//tail位置のボーン番号 (ない場合は0xFFFF
	unsigned char boneType;				//ボーンの種類 0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 5:回転影響下 6:IK接続先 7:非表示 8:捻り 9:回転運動 (8, 9はMMD4.0以降)
	unsigned short ikParentBoneIndex;	//IKボーン番号 (ない場合は0
	Vector3D boneHeadPos;				//ボーンのヘッド位置
};

#pragma pack(pop)

// .pmx
// PMXのモデルデータ型
struct PMXModelData{
	static constexpr int NO_DATA_FLAG = -1;

	struct Vertex
	{
		// 頂点座標
		Vector3D position;
		// 法線
		Vector3D normal;
		// UV座標
		Vector2D uv;
		// 追加UV座標
		std::vector<Vector4D> additionalUV;

		// ボーンウェイト
		struct Weight
		{
			enum Type
			{
				BDEF1,
				BDEF2,
				BDEF4,
				SDEF,
			};

			Type type;

			int born1;
			int	born2;
			int	born3;
			int	born4;

			float weight1;
			float weight2;
			float weight3;
			float weight4;

			Vector3D c;
			Vector3D r0;
			Vector3D r1;
		} weight;

		// エッジ倍率
		float edgeMagnif;
	};

	struct Surface
	{
		unsigned short vertexIndex;
	};

	struct Material
	{
		Vector4D diffuse;
		Vector3D specular;
		float specularity;
		Vector3D ambient;

		int colorMapTextureIndex;
		int toonTextureIndex;
		// (スフィアテクスチャは非対応)

		// 材質ごとの頂点数
		int vertexNum;
	};

	struct Bone
	{
		// ボーン名
		std::string name;

		// English version
		std::string nameEnglish;

		Vector3D position;

		int parentIndex;

		int transformationLevel;

		unsigned short flag;

		Vector3D coordOffset;

		int childrenIndex;
		int impartParentIndex;
		float impartRate;

		// 固定軸方向ベクトル
		Vector3D fixedAxis;

		// ローカルのX軸方向ベクトル
		Vector3D localAxisX;

		// ローカルのZ軸方向ベクトル
		Vector3D localAxisZ;

		int externalParentKey;

		int ikTargetIndex;
		int ikLoopCount;
		float ikUnitAngle;

		struct IKLink
		{
			int index;
			bool existAngleLimited;
			Vector3D limitAngleMin;
			Vector3D limitAngleMax;
		};

		std::vector<IKLink> ikLinks;
	};

	std::vector<Vertex> vertices;
	std::vector<Surface> surfaces;
	std::vector<std::string> texturePaths;
	std::vector<Material> materials;
	std::vector<Bone> bones;
};

//マテリアル構造体
struct PmxMaterial
{
	//Vector4D diffuse;
	//Vector3D specular;
	//float specularity;
	//Vector3D ambient;

	int colorMapTextureIndex;
	//int toonTextureIndex;

	int vertexNum;
};

// .vmd
#pragma pack(push,1)

// VMDのヘッダー
struct VmdHeader
{
	char vmdHeader[30];		//"Vocaloid Motion Data 0002"
	char vmdModelName[20];  // カメラの場合:"カメラ・照明"
};

//モーション
struct VmdMotionData
{
	char boneName[15];			//ボーン名
	unsigned long frameNo;		//フレーム番号
	float location[3];			// 移動量
	Quaternion quaternion;		// 回転量 (モデルローカル座標系
	unsigned char bezier[64];	// 補完
};

// 表情データ
struct VmdSkin
{
	char skinName[15];		// 表情名
	unsigned long flameNo;	// フレーム番号
	float weight;			// 表情の設定値(表情スライダーの値)
};

#pragma pack(pop)

// ボーン
struct VmdBone
{
	VmdBone() : type(0), index(-1)
	{
		std::vector<VmdBone*>().swap(vecChildrenPtr);
	};

	long index;							// ボーンID（通し番号）
	std::string name;				// ボーン名
	unsigned char type;				// ボーンタイプ (MMDの場合 0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 5:回転影響下 6:IK接続先 7:非表示 8:捻り 9:回転運動 )
	Vector3D pos;
	std::vector<VmdBone*> vecChildrenPtr;
};

#endif	// #ifndef INCLUDE_EIDOS_EIDOSTYPE_H