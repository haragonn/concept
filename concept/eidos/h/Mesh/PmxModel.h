#ifndef INCLUDE_EIDOS_PMXMODEL_H
#define INCLUDE_EIDOS_PMXMODEL_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
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

// PMX�̃��f���f�[�^�^
struct PMXModelData
{
	static constexpr int NO_DATA_FLAG = -1;

	struct Vertex
	{
		// ���_���W
		XMFLOAT3 position;
		// �@��
		XMFLOAT3 normal;
		// UV���W
		XMFLOAT2 uv;
		// �ǉ�UV���W
		std::vector<XMFLOAT4> additionalUV;

		// �{�[���E�F�C�g
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

		// �G�b�W�{��
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
		// (�X�t�B�A�e�N�X�`���͔�Ή�)

		// �ގ����Ƃ̒��_��
		int vertexNum;
	};

	struct Bone
	{
		// �{�[����
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
		// �Œ莲�����x�N�g��
		XMFLOAT3 fixedAxis;
		// ���[�J����X�������x�N�g��
		XMFLOAT3 localAxisX;
		// ���[�J����Z�������x�N�g��
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
	bool LoadPmxMeshFromFile(const char* pFileName);//�ǂݍ���
	bool LoadPmxMeshFromStorage(const char* pFileName);	// �X�g���[�W����ǂݍ���
	void UnLoad();

private:
	friend class VmdMotion;

	ID3D11Buffer* pVertexBuffer_;//���_�o�b�t�@
	ID3D11Buffer* pIndexBuffer_;//�C���f�b�N�X�o�b�t�@
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

	bool bStorage_;		// �X�g���[�W�g�p�t���O

	void Draw(Camera* pCamera)override;//�`��

	// �R�s�[�R���X�g���N�^�̋֎~
	PmxModel(const PmxModel& src){}
	PmxModel& operator=(const PmxModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_PMXMODEL_H
