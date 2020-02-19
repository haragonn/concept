#ifndef INCLUDE_EIDOS_FBXMESH_H
#define INCLUDE_EIDOS_FBXMESH_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <fbxsdk.h>
#include <vector>
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"
#include "../../../eidos/h/3D/Object.h"
#include "../../../eidos//h//Mesh/ObjModel.h"

class FbxMeshHolder;
//class FbxManager;
//class FbxScene;
//class FbxMesh;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;
//
////�}�e���A���\����
//struct ObjMaterial
//{
//	char name[256];
//	Vector3D ambient;
//	Vector3D diffuse;
//	Vector3D specular;
//	float shininess;
//	float alpha;
//	char ambientMapName[256];
//	char diffuseMapName[256];
//	char specularMapName[256];
//	char bumpMapName[256];
//};
//
//struct ObjSubset
//{
//	unsigned int materialIndex;
//	unsigned int faceStart;
//	unsigned int faceCount;
//};
////���_�\����
//struct VertexDataMesh
//{
//	Vector3D pos;
//	Vector3D nor;
//	Vector2D tex;
//};

class FbxModel : public Object{
public:
	FbxModel();
	~FbxModel();
	bool LoadFbxMeshFromFile(const char* pFileName);//�ǂݍ���
	bool LoadFbxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//�ǂݍ���
	//bool LoadFbxMeshFromStorage(const char* pFileName);	// �X�g���[�W����ǂݍ���
	void UnLoad();
	ID3D11Buffer* GetVertexBufferPtr()const{ return pVertexBuffer_; }
	ID3D11Buffer* GetIndexBufferPtr()const{ return pIndexBuffer_; }
	unsigned int GetVertexSize()const{ return vertexSize_; };
	ObjSubset* GetSubsetPtr(){ return &vecSubset_[0]; }
	unsigned int GetSubsetSize()const{ return subsetSize_; };
	Texture** GetTexturePtrPtr(){ return &vecTexPtr_[0]; }
	unsigned int GetTexturePtrSize()const{ return texPtrSize_; };

private:
	FbxManager* pFbxManager_;
	FbxScene* pFbxScene_;
	FbxMesh* pFbxMesh_;
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

	void SetupNode(FbxNode* pNode, std::string parentName, int faceStart, const char * pFileName);

	void Draw(Camera* pCamera)override;//�`��
	//bool LoadMaterialFromFile(const char* pFileName);//�}�e���A���t�@�C���ǂݍ���
	//bool LoadMaterialFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//�}�e���A���t�@�C���ǂݍ���

																						  // �R�s�[�R���X�g���N�^�̋֎~
	FbxModel(const FbxModel& src){}
	FbxModel& operator=(const FbxModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_FBXMESH_H
