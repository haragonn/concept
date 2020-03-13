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

// ���_�\����
struct MeshVertexData
{
	Vector3D pos;
	Vector3D nor;
	Color color;
	Vector2D tex;
};

// �u�����h�d�ݕt���b�V���̒��_�f�[�^
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

// �T�u�Z�b�g
struct ObjSubset
{
	unsigned int materialIndex;
	unsigned int faceStart;
	unsigned int faceCount;
};

// �}�e���A���\����
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
//�}�e���A���\����
struct FbxMaterial
{
	char* pMaterialName;
	Color color;
	const char* pFileName;
	Texture* pTexture;
};

//���_�\����
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

// PMD�t�@�C���̃w�b�_�[
struct PmdHeader
{
	unsigned char magic[3];			//"Pmd"
	float version;					//00 00 80 3F
	unsigned char modelName[20];	//���f����
	unsigned char comment[256];		//���f���t���R�����g
};

// PMD�t�@�C������ǂݍ��ޒ��_���
struct PmdVertex
{
	Vector3D pos;
	Vector3D nor;
	Vector2D tex;
	unsigned short boneNum[2];		//�e���{�[���ԍ�
	unsigned char boneWeight;		//�e���x
	unsigned char edgeFlag;			//�G�b�W
};

// PMD�t�@�C������ǂݍ��ރ}�e���A�����
struct PmdMaterial
{
	float diffuse[3];			//�����F
	float alpha;				//������
	float specularity;			//���򃌃x��
	float specular[3];			//����F
	float ambient[3];			//���F
	unsigned char toonIndex;	//toon
	unsigned char edgeFlag;		//�֊s
	unsigned long faceCount;	// ���̍ޗ��̖ʒ��_��
	char textureFileName[20];	//�e�N�X�`���t�@�C���l�[��
};

// PDM�t�@�C������ǂݍ��ރ{�[�����
struct PmdBone
{
	char boneName[20];					//�{�[����
	unsigned short parentBoneIndex;	//�e�{�[���ԍ� (�Ȃ��ꍇ��0xFFFF
	unsigned short tailPosBoneIndex;	//tail�ʒu�̃{�[���ԍ� (�Ȃ��ꍇ��0xFFFF
	unsigned char boneType;				//�{�[���̎�� 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^�� (8, 9��MMD4.0�ȍ~)
	unsigned short ikParentBoneIndex;	//IK�{�[���ԍ� (�Ȃ��ꍇ��0
	Vector3D boneHeadPos;				//�{�[���̃w�b�h�ʒu
};

#pragma pack(pop)

// .pmx
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

//�}�e���A���\����
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

// VMD�̃w�b�_�[
struct VmdHeader
{
	char vmdHeader[30];		//"Vocaloid Motion Data 0002"
	char vmdModelName[20];  // �J�����̏ꍇ:"�J�����E�Ɩ�"
};

//���[�V����
struct VmdMotionData
{
	char boneName[15];			//�{�[����
	unsigned long frameNo;		//�t���[���ԍ�
	float location[3];			// �ړ���
	Quaternion quaternion;		// ��]�� (���f�����[�J�����W�n
	unsigned char bezier[64];	// �⊮
};

// �\��f�[�^
struct VmdSkin
{
	char skinName[15];		// �\�
	unsigned long flameNo;	// �t���[���ԍ�
	float weight;			// �\��̐ݒ�l(�\��X���C�_�[�̒l)
};

#pragma pack(pop)

// �{�[��
struct VmdBone
{
	VmdBone() : type(0), index(-1)
	{
		std::vector<VmdBone*>().swap(vecChildrenPtr);
	};

	long index;							// �{�[��ID�i�ʂ��ԍ��j
	std::string name;				// �{�[����
	unsigned char type;				// �{�[���^�C�v (MMD�̏ꍇ 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^�� )
	Vector3D pos;
	std::vector<VmdBone*> vecChildrenPtr;
};

#endif	// #ifndef INCLUDE_EIDOS_EIDOSTYPE_H