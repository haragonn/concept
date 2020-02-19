#include "../../h/eidosStorage/eidosStorage.h"
#include "../../h/eidosStorage/eidosStorageManager.h"

bool eidosStorage::LoadObjMeshFromFile(const char * pFileName)
{
	return eidosStorageManager::Instance().LoadObjMeshFromFile(pFileName);
}

bool eidosStorage::LoadObjMeshFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	return eidosStorageManager::Instance().LoadObjMeshFromArchiveFile(pArchiveFileName, pFileName);
}

bool eidosStorage::LoadPmdMeshFromFile(const char * pFileName)
{
	return eidosStorageManager::Instance().LoadPmdMeshFromFile(pFileName);
}

//bool eidosStorage::LoadObjPmdFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
//{
//	return eidosStorageManager::Instance().LoadPmdMeshFromArchiveFile(pArchiveFileName, pFileName);
//}

void eidosStorage::UnLoadObjMesh(const char * pFileName)
{
	return eidosStorageManager::Instance().SetUnLoadObjModelRequest(pFileName);
}

void eidosStorage::UnLoadPmdMesh(const char * pFileName)
{
	return eidosStorageManager::Instance().SetUnLoadPmdModelRequest(pFileName);
}

void eidosStorage::UnLoadAll()
{
	eidosStorageManager::Instance().SetUnLoadAllRequest(true);
}
