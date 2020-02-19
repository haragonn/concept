#ifndef INCLUDE_IDEA_STORAGEMANAGER_H
#define INCLUDE_IDEA_STORAGEMANAGER_H

#include <vector>
#include <unordered_map>

class Texture;
class Sound;

class StorageManager{
public:
	static StorageManager& Instance()
	{
		static StorageManager s_Instance;
		return s_Instance;
	}

	bool LoadImageFromFile(const char* pFileName);	// 画像の読み込み
	bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルから画像を読み込む
	Texture& GetTexture(const char* pFileName);	// 画像の取得
	bool HasKeyTexture(const char* pFileName);	// 指定したファイル名と関連付けられた画像が存在するか

	bool LoadWaveFromFile(const char* pFileName, unsigned int soundSize = 1U);		// waveファイルの読み込み
	bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize = 1U);	// アーカイブファイルからwaveファイルを読み込む
	Sound& GetSound(const char* pFileName);		// サウンドの取得
	bool HasKeyWave(const char* pFileName);		// 指定したファイル名と関連付けられたwaveデータが存在するか

	void SetUnLoadImageRequest(const char* pFileName);
	inline int GetUnLoadImageRequestSize()const{ return vecUnLoadImageList_.size(); }
	void UnLoadImage();

	void SetUnLoadWaveRequest(const char* pFileName);
	inline int GetUnLoadWaveRequestSize()const{ return vecUnLoadWaveList_.size(); }
	void UnLoadWave();

	inline void SetUnLoadAllRequest(bool bRequest){ bUnLoadAll_ = bRequest; }
	inline bool IsUnLoadAllRequest()const{ return bUnLoadAll_; }
	void UnLoadAll();	// 全ての画像とソースボイスの破棄

private:
	std::unordered_map<const char*, Texture*> umTexturePtr_;
	std::vector<const char*> vecUnLoadImageList_;
	static Texture s_NullTexture_;

	std::unordered_map<const char*, Sound*> umSoundPtr_;
	std::unordered_map<const char*, unsigned int> umSoundSize_;
	std::unordered_map<const char*, unsigned int> umSoundIndex_;
	std::vector<const char*> vecUnLoadWaveList_;
	static Sound s_NullSound_;

	bool bUnLoadAll_;

	StorageManager();					// コンストラクタ
	~StorageManager(){ UnLoadAll(); }	// デストラクタ
	// コピーコンストラクタの禁止
	StorageManager(const StorageManager& src){}
	StorageManager& operator=(const StorageManager& src){}
};

#endif	// #ifndef INCLUDE_IDEA_STORAGEMANAGER_H
