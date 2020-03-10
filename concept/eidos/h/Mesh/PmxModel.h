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

struct ID3D11Buffer;

// PMX�̃��f���f�[�^�^
struct PMXModelData{
	static constexpr int NO_DATA_FLAG = -1;

	struct Vertex
	{
		// ���_���W
		Vector3D position;
		// �@��
		Vector3D normal;
		// UV���W
		Vector2D uv;
		// �ǉ�UV���W
		std::vector<Vector4D> additionalUV;

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

			Vector3D c;
			Vector3D r0;
			Vector3D r1;
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
		Vector4D diffuse;
		Vector3D specular;
		float specularity;
		Vector3D ambient;

		int colorMapTextureIndex;
		int toonTextureIndex;
		// (�X�t�B�A�e�N�X�`���͔�Ή�)

		// �ގ����Ƃ̒��_��
		int vertexNum;
	};

	struct Bone
	{
		// �{�[����
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

		// �Œ莲�����x�N�g��
		Vector3D fixedAxis;

		// ���[�J����X�������x�N�g��
		Vector3D localAxisX;

		// ���[�J����Z�������x�N�g��
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
	unsigned long materialSize_;
	std::map<std::string, VmdBone> mapBone_;
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
