#include "../../h/Storage/Storage.h"
#include "../../h/Storage/StorageManager.h"
#include "../../../eidos/h/eidosStorage/eidosStorageManager.h"

//------------------------------------------------------------------------------
// �摜�̓ǂݍ���
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Storage::LoadImageFromFile(const char * pFileName)
{
	return StorageManager::Instance().LoadImageFromFile(pFileName);
}

//------------------------------------------------------------------------------
// �A�[�J�C�u�t�@�C������摜��ǂݍ���
// �����@�F�A�[�J�C�u�t�@�C����(const char* pArchiveFileName),
// �@�@�@�@�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Storage::LoadImageFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	return StorageManager::Instance().LoadImageFromArchiveFile(pArchiveFileName, pFileName);
}

bool Storage::LoadObjMeshFromFile(const char * pFileName)
{
	return eidosStorageManager::Instance().LoadObjMeshFromFile(pFileName);
}

bool Storage::LoadObjMeshFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	return eidosStorageManager::Instance().LoadObjMeshFromArchiveFile(pArchiveFileName, pFileName);
}

bool Storage::LoadPmdMeshFromFile(const char* pFileName)
{
	return eidosStorageManager::Instance().LoadPmdMeshFromFile(pFileName);
}

bool Storage::LoadPmxMeshFromFile(const char* pFileName)
{
	return eidosStorageManager::Instance().LoadPmxMeshFromFile(pFileName);
}

//------------------------------------------------------------------------------
// wave�t�@�C���̓ǂݍ���
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Storage::LoadWaveFromFile(const char * pFileName, unsigned int soundSize)
{
	return StorageManager::Instance().LoadWaveFromFile(pFileName, soundSize);
}

//------------------------------------------------------------------------------
// �A�[�J�C�u�t�@�C������wave�t�@�C����ǂݍ���
// �����@�F�A�[�J�C�u�t�@�C����(const char* pArchiveFileName),
// �@�@�@�@�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Storage::LoadWaveFromArchiveFile(const char * pArchiveFileName, const char * pFileName, unsigned int soundSize)
{
	return StorageManager::Instance().LoadWaveFromArchiveFile(pArchiveFileName, pFileName, soundSize);
}

//------------------------------------------------------------------------------
// �摜�̔j��
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Storage::UnLoadImage(const char * pFileName)
{
	return StorageManager::Instance().SetUnLoadImageRequest(pFileName);
}

void Storage::UnLoadObjMesh(const char * pFileName)
{
	return eidosStorageManager::Instance().SetUnLoadObjModelRequest(pFileName);
}

void Storage::UnLoadPmdMesh(const char* pFileName)
{
	return eidosStorageManager::Instance().SetUnLoadPmdModelRequest(pFileName);
}

void Storage::UnLoadPmxMesh(const char* pFileName)
{
	return eidosStorageManager::Instance().SetUnLoadPmxModelRequest(pFileName);
}

//------------------------------------------------------------------------------
// �\�[�X�{�C�X�̔j��
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Storage::UnLoadWave(const char * pFileName)
{
	return StorageManager::Instance().SetUnLoadWaveRequest(pFileName);
}

//------------------------------------------------------------------------------
// �S�Ẳ摜�ƃ\�[�X�{�C�X�̔j��
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Storage::UnLoadAll()
{
	StorageManager::Instance().SetUnLoadAllRequest(true);
	eidosStorageManager::Instance().SetUnLoadAllRequest(true);
}
