#ifndef INCLUDE_IDEA_STORAGE_H
#define INCLUDE_IDEA_STORAGE_H

class Storage{
public:
	static bool LoadImageFromFile(const char* pFileName);	// �摜�̓ǂݍ���
	static bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName);		// �A�[�J�C�u�t�@�C������摜��ǂݍ���
	static void UnLoadImage(const char* pFileName);			// �摜�̔j��

	static bool LoadObjMeshFromFile(const char* pFileName);	// OBJ���b�V���̓ǂݍ���
	static bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������OBJ���b�V����ǂݍ���
	static void UnLoadObjMesh(const char* pFileName);		// OBJ���b�V���̔j��

	static bool LoadPmdMeshFromFile(const char* pFileName);	// PMD���b�V���̓ǂݍ���
	//static bool LoadPmdMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������PMD���b�V����ǂݍ���
	static void UnLoadPmdMesh(const char* pFileName);		// PMD���b�V���̔j��

	static bool LoadPmxMeshFromFile(const char* pFileName);	// PMX���b�V���̓ǂݍ���
	//static bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������PMX���b�V����ǂݍ���
	static void UnLoadPmxMesh(const char* pFileName);		// PMX���b�V���̔j��

	static bool LoadWaveFromFile(const char* pFileName, unsigned int soundSize = 1U);	// wave�t�@�C���̓ǂݍ���
	static bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize = 1U);	// �A�[�J�C�u�t�@�C������wave�t�@�C����ǂݍ���
	static void UnLoadWave(const char* pFileName);			// wave�t�@�C���̔j��


	static void UnLoadAll();	// �S�Ẵ��\�[�X�̔j��
};

#endif	// #ifndef INCLUDE_IDEA_STORAGE_H