#ifndef INCLUDE_EIDOS_PMXMODEL_H
#define INCLUDE_EIDOS_PMXMODEL_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../Utility/eidosType.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"

struct ID3D11Buffer;


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
