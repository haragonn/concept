#ifndef INCLUDE_EIDOS_PMDMODEL_H
#define INCLUDE_EIDOS_PMDMODEL_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"

#include <vector>
#include <map>
#include <string>

struct ID3D11Buffer;

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
	unsigned short parentBoneIndex	;	//�e�{�[���ԍ� (�Ȃ��ꍇ��0xFFFF
	unsigned short tailPosBoneIndex;	//tail�ʒu�̃{�[���ԍ� (�Ȃ��ꍇ��0xFFFF
	unsigned char boneType;				//�{�[���̎�� 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^�� (8, 9��MMD4.0�ȍ~)
	unsigned short ikParentBoneIndex;	//IK�{�[���ԍ� (�Ȃ��ꍇ��0
	Vector3D boneHeadPos;				//�{�[���̃w�b�h�ʒu
};

#pragma pack(pop)

// �{�[��
struct Bone 
{
	Bone() : type(0), index(-1)
	{
		std::vector<Bone*>().swap(vecChildrenPtr);
	};

	long index;							// �{�[��ID�i�ʂ��ԍ��j
	std::string name;				// �{�[����
	unsigned char type;				// �{�[���^�C�v (MMD�̏ꍇ 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^�� )
	Vector3D pos;
	std::vector<Bone*> vecChildrenPtr;
};

class PmdModel : public Object{
public:
	PmdModel();
	~PmdModel();
	bool LoadPmdMeshFromFile(const char* pFileName);//�ǂݍ���
	bool LoadPmdMeshFromStorage(const char* pFileName);	// �X�g���[�W����ǂݍ���
	void UnLoad();

private:
	friend class VmdMotion;

	ID3D11Buffer* pVertexBuffer_;//���_�o�b�t�@
	ID3D11Buffer* pIndexBuffer_;//�C���f�b�N�X�o�b�t�@

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
	PmdModel(const PmdModel& src){}
	PmdModel& operator=(const PmdModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_PMDMODEL_H