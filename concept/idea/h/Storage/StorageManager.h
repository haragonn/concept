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

	bool LoadImageFromFile(const char* pFileName);	// �摜�̓ǂݍ���
	bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������摜��ǂݍ���
	Texture& GetTexture(const char* pFileName);	// �摜�̎擾
	bool HasKeyTexture(const char* pFileName);	// �w�肵���t�@�C�����Ɗ֘A�t����ꂽ�摜�����݂��邩

	bool LoadWaveFromFile(const char* pFileName, unsigned int soundSize = 1U);		// wave�t�@�C���̓ǂݍ���
	bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize = 1U);	// �A�[�J�C�u�t�@�C������wave�t�@�C����ǂݍ���
	Sound& GetSound(const char* pFileName);		// �T�E���h�̎擾
	bool HasKeyWave(const char* pFileName);		// �w�肵���t�@�C�����Ɗ֘A�t����ꂽwave�f�[�^�����݂��邩

	void SetUnLoadImageRequest(const char* pFileName);
	inline int GetUnLoadImageRequestSize()const{ return vecUnLoadImageList_.size(); }
	void UnLoadImage();

	void SetUnLoadWaveRequest(const char* pFileName);
	inline int GetUnLoadWaveRequestSize()const{ return vecUnLoadWaveList_.size(); }
	void UnLoadWave();

	inline void SetUnLoadAllRequest(bool bRequest){ bUnLoadAll_ = bRequest; }
	inline bool IsUnLoadAllRequest()const{ return bUnLoadAll_; }
	void UnLoadAll();	// �S�Ẳ摜�ƃ\�[�X�{�C�X�̔j��

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

	StorageManager();					// �R���X�g���N�^
	~StorageManager(){ UnLoadAll(); }	// �f�X�g���N�^
	// �R�s�[�R���X�g���N�^�̋֎~
	StorageManager(const StorageManager& src){}
	StorageManager& operator=(const StorageManager& src){}
};

#endif	// #ifndef INCLUDE_IDEA_STORAGEMANAGER_H
