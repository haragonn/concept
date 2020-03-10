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

struct ID3D11Buffer;

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
	unsigned long materialSize_;
	std::map<std::string, VmdBone> mapBone_;
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
