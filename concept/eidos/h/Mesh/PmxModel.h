#ifndef INCLUDE_EIDOS_PMXMODEL_H
#define INCLUDE_EIDOS_PMXMODEL_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "PmdModel.h"

#include <vector>
#include <map>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;


struct ID3D11Buffer;

// PMXのモデルデータ型
struct PMXModelData
{
	static constexpr int NO_DATA_FLAG = -1;

	struct Vertex
	{
		// 頂点座標
		XMFLOAT3 position;
		// 法線
		XMFLOAT3 normal;
		// UV座標
		XMFLOAT2 uv;
		// 追加UV座標
		std::vector<XMFLOAT4> additionalUV;

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
			XMFLOAT3 c;
			XMFLOAT3 r0;
			XMFLOAT3 r1;
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
		XMFLOAT4 diffuse;
		XMFLOAT3 specular;
		float specularity;
		XMFLOAT3 ambient;

		int colorMapTextureIndex;
		int toonTextureIndex;
		// (スフィアテクスチャは非対応)

		// 材質ごとの頂点数
		int vertexNum;
	};

	struct Bone
	{
		// ボーン名
		std::wstring name;
		// English version
		std::string nameEnglish;
		XMFLOAT3 position;
		int parentIndex;
		int transformationLevel;
		unsigned short flag;
		XMFLOAT3 coordOffset;
		int childrenIndex;
		int impartParentIndex;
		float impartRate;
		// 固定軸方向ベクトル
		XMFLOAT3 fixedAxis;
		// ローカルのX軸方向ベクトル
		XMFLOAT3 localAxisX;
		// ローカルのZ軸方向ベクトル
		XMFLOAT3 localAxisZ;
		int externalParentKey;
		int ikTargetIndex;
		int ikLoopCount;
		float ikUnitAngle;
		struct IKLink
		{
			int index;
			bool existAngleLimited;
			XMFLOAT3 limitAngleMin;
			XMFLOAT3 limitAngleMax;
		};
		std::vector<IKLink> ikLinks;
	};


	std::vector<Vertex> vertices;
	std::vector<Surface> surfaces;
	std::vector<std::wstring> texturePaths;
	std::vector<Material> materials;
	std::vector<Bone> bones;
};

class PmxModel : public Object{
public:
	PmxModel();
	~PmxModel();
	bool LoadPmxMeshFromFile(const char* pFileName);//読み込み
	bool LoadPmxMeshFromStorage(const char* pFileName);	// ストレージから読み込む
	void UnLoad();

private:
	friend class VmdMotion;

	ID3D11Buffer* pVertexBuffer_;//頂点バッファ
	ID3D11Buffer* pIndexBuffer_;//インデックスバッファ
	PMXModelData pmxData_;
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
	PmxModel(const PmxModel& src){}
	PmxModel& operator=(const PmxModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_PMXMODEL_H
