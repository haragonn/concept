#ifndef INCLUDE_EIDOS_EIDOSSTORAGE_H
#define INCLUDE_EIDOS_EIDOSSTORAGE_H

class eidosStorage{
public:
	static bool LoadObjMeshFromFile(const char* pFileName);	// OBJ���b�V���̓ǂݍ���
	static bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������OBJ���b�V����ǂݍ���
	static bool LoadPmdMeshFromFile(const char* pFileName);	// PMD���b�V���̓ǂݍ���
	static bool LoadPmdMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������PMD���b�V����ǂݍ���

	static void UnLoadObjMesh(const char* pFileName);	// OBJ���b�V���̔j��
	static void UnLoadPmdMesh(const char* pFileName);	// PMD���b�V���̔j��
	static void UnLoadAll();	// �S�Ẵ��b�V���̔j��
};

#endif	// #ifndef INCLUDE_EIDOS_EIDOSSTORAGE_H