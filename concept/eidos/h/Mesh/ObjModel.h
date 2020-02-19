#ifndef INCLUDE_EIDOS_OBJMODEL_H
#define INCLUDE_EIDOS_OBJMODEL_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../idea/h/Utility/ideaUtility.h"
#include <vector>

//class ObjMeshHolder;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;

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

class ObjModel : public Object{
public:
	ObjModel();
	~ObjModel();
	bool LoadObjMeshFromFile(const char* pFileName);//�ǂݍ���
	bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//�ǂݍ���
	bool LoadObjMeshFromStorage(const char* pFileName);	// �X�g���[�W����ǂݍ���
	void UnLoad();

private:
	ID3D11Buffer* pVertexBuffer_;//���_�o�b�t�@
	ID3D11Buffer* pIndexBuffer_;//�C���f�b�N�X�o�b�t�@
	std::vector<MeshVertexData> vecVertex_;
	std::vector<ObjSubset> vecSubset_;
	std::vector<ObjMaterial> vecMaterial_;
	std::vector<unsigned short> vecIndex_;
	unsigned int vertexSize_;
	unsigned int indexSize_;
	unsigned int subsetSize_;
	unsigned int materialSize_;
	unsigned int texPtrSize_;
	std::vector<Texture*> vecTexPtr_;
	bool bStorage_;		// �X�g���[�W�g�p�t���O

	bool LoadMaterialFromFile(const char* pFileName);//�}�e���A���t�@�C���ǂݍ���
	bool LoadMaterialFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//�}�e���A���t�@�C���ǂݍ���

	void Draw(Camera* pCamera)override;

	// �R�s�[�R���X�g���N�^�̋֎~
	ObjModel(const ObjModel& src){}
	ObjModel& operator=(const ObjModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_OBJMODEL_H
