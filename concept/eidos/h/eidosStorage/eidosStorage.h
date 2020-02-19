#ifndef INCLUDE_EIDOS_EIDOSSTORAGE_H
#define INCLUDE_EIDOS_EIDOSSTORAGE_H

class eidosStorage{
public:
	static bool LoadObjMeshFromFile(const char* pFileName);	// OBJメッシュの読み込み
	static bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルからOBJメッシュを読み込む
	static bool LoadPmdMeshFromFile(const char* pFileName);	// PMDメッシュの読み込み
	static bool LoadPmdMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルからPMDメッシュを読み込む

	static void UnLoadObjMesh(const char* pFileName);	// OBJメッシュの破棄
	static void UnLoadPmdMesh(const char* pFileName);	// PMDメッシュの破棄
	static void UnLoadAll();	// 全てのメッシュの破棄
};

#endif	// #ifndef INCLUDE_EIDOS_EIDOSSTORAGE_H