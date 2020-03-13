#ifndef INCLUDE_IDEA_STORAGE_H
#define INCLUDE_IDEA_STORAGE_H

class Storage{
public:
	static bool LoadImageFromFile(const char* pFileName);	// 画像の読み込み
	static bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName);		// アーカイブファイルから画像を読み込む
	static void UnLoadImage(const char* pFileName);			// 画像の破棄

	static bool LoadObjMeshFromFile(const char* pFileName);	// OBJメッシュの読み込み
	static bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルからOBJメッシュを読み込む
	static void UnLoadObjMesh(const char* pFileName);		// OBJメッシュの破棄

	static bool LoadPmdMeshFromFile(const char* pFileName);	// PMDメッシュの読み込み
	//static bool LoadPmdMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルからPMDメッシュを読み込む
	static void UnLoadPmdMesh(const char* pFileName);		// PMDメッシュの破棄

	static bool LoadPmxMeshFromFile(const char* pFileName);	// PMXメッシュの読み込み
	//static bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルからPMXメッシュを読み込む
	static void UnLoadPmxMesh(const char* pFileName);		// PMXメッシュの破棄

	static bool LoadWaveFromFile(const char* pFileName, unsigned int soundSize = 1U);	// waveファイルの読み込み
	static bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize = 1U);	// アーカイブファイルからwaveファイルを読み込む
	static void UnLoadWave(const char* pFileName);			// waveファイルの破棄


	static void UnLoadAll();	// 全てのリソースの破棄
};

#endif	// #ifndef INCLUDE_IDEA_STORAGE_H