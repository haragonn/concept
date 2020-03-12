#ifndef INCLUDE_EIDOS_FBXMESH_H
#define INCLUDE_EIDOS_FBXMESH_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <fbxsdk.h>
#include <vector>
#include "../../../eidos/h/3D/Object.h"
#include "../../../eidos/h/3D/ObjectManager.h"
#include "../../../eidos/h/Utility/eidosType.h"
#include "../../../idea/h/Texture/Texture.h"
#include "../../../idea/h/Utility/ideaMath.h"

struct ID3D11ShaderResourceView;
struct ID3D11Buffer;

class FbxModel : public Object{
public:
	FbxModel();
	~FbxModel();

	bool LoadFbxMeshFromFile(const char* pFileName);//�ǂݍ���
	bool LoadFbxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);//�ǂݍ���
	//bool LoadFbxMeshFromStorage(const char* pFileName);	// �X�g���[�W����ǂݍ���

	void UnLoad();

private:
	FbxManager* pFbxManager_;
	FbxScene* pFbxScene_;
	FbxMesh* pFbxMesh_;

	ID3D11Buffer* pVertexBuffer_;//���_�o�b�t�@
	ID3D11Buffer* pIndexBuffer_;//�C���f�b�N�X�o�b�t�@

	std::vector<ID3D11Buffer*> vecVertexBufferPtr_;
	std::vector<ID3D11Buffer*> vecIndexBufferPtr_;

	std::vector<MeshVertexData> vecVertex_;
	unsigned int vertexSize_;

	std::vector<unsigned short> vecIndex_;
	unsigned int indexSize_;

	std::vector<FbxMeshData> vecMesh_;
	unsigned int meshSize_;

	std::vector<FbxMaterial> vecMaterial_;
	unsigned int materialSize_;

	std::vector<Texture*> vecTexPtr_;
	unsigned int texPtrSize_;

	bool bStorage_;		// �X�g���[�W�g�p�t���O

	void SetupNode(FbxNode* pNode, std::string parentName, int faceStart, const char * pFileName);

	void Draw(Camera* pCamera)override;//�`��

	// �R�s�[�R���X�g���N�^�̋֎~
	FbxModel(const FbxModel& src){}
	FbxModel& operator=(const FbxModel& src){}
};

#endif	// #ifndef INCLUDE_EIDOS_FBXMESH_H
