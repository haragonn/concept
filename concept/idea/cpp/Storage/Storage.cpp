#include "../../h/Storage/Storage.h"
#include "../../h/Storage/StorageManager.h"
#include "../../../eidos/h/eidosStorage/eidosStorageManager.h"

//------------------------------------------------------------------------------
// 画像の読み込み
// 引数　：ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Storage::LoadImageFromFile(const char * pFileName)
{
	return StorageManager::Instance().LoadImageFromFile(pFileName);
}

//------------------------------------------------------------------------------
// アーカイブファイルから画像を読み込む
// 引数　：アーカイブファイル名(const char* pArchiveFileName),
// 　　　　ファイル名(const char* pFileName)
// 戻り値：成否
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
// waveファイルの読み込み
// 引数　：ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Storage::LoadWaveFromFile(const char * pFileName, unsigned int soundSize)
{
	return StorageManager::Instance().LoadWaveFromFile(pFileName, soundSize);
}

//------------------------------------------------------------------------------
// アーカイブファイルからwaveファイルを読み込む
// 引数　：アーカイブファイル名(const char* pArchiveFileName),
// 　　　　ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Storage::LoadWaveFromArchiveFile(const char * pArchiveFileName, const char * pFileName, unsigned int soundSize)
{
	return StorageManager::Instance().LoadWaveFromArchiveFile(pArchiveFileName, pFileName, soundSize);
}

//------------------------------------------------------------------------------
// 画像の破棄
// 引数　：ファイル名(const char* pFileName)
// 戻り値：なし
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
// ソースボイスの破棄
// 引数　：ファイル名(const char* pFileName)
// 戻り値：なし
//------------------------------------------------------------------------------
void Storage::UnLoadWave(const char * pFileName)
{
	return StorageManager::Instance().SetUnLoadWaveRequest(pFileName);
}

//------------------------------------------------------------------------------
// 全ての画像とソースボイスの破棄
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Storage::UnLoadAll()
{
	StorageManager::Instance().SetUnLoadAllRequest(true);
	eidosStorageManager::Instance().SetUnLoadAllRequest(true);
}
