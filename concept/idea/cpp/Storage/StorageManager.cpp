/*==============================================================================
	[StorageManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Storage/StorageManager.h"
#include "../../h/Texture/Texture.h"
#include "../../h/Sound/Sound.h"
#include "../../h/Utility/ideaUtility.h"

using namespace std;

Texture StorageManager::s_NullTexture_;
Sound StorageManager::s_NullSound_;

StorageManager::StorageManager()
	: bUnLoadAll_(false)	
{
	unordered_map<const char*, Texture*>().swap(umTexturePtr_);
	vector<const char*>().swap(vecUnLoadImageList_);

	unordered_map<const char*, Sound*>().swap(umSoundPtr_);
	unordered_map<const char*, unsigned int>().swap(umSoundSize_);
	unordered_map<const char*, unsigned int>().swap(umSoundIndex_);
	vector<const char*>().swap(vecUnLoadWaveList_);
}

//------------------------------------------------------------------------------
// �摜�̓ǂݍ���
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool StorageManager::LoadImageFromFile(const char * pFileName)
{
	if(HasKeyTexture(pFileName)){ return false; }	// ���ɃL�[���o�^����Ă���ΏI��

	Texture* pTex = new Texture;
	bool bRes = pTex->LoadImageFromFile(pFileName);

	if(bRes){
		umTexturePtr_.insert(pair<const char*, Texture*>{ pFileName, pTex });
	} else{
		SafeDelete(pTex);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// �A�[�J�C�u�t�@�C������摜��ǂݍ���
// �����@�F�A�[�J�C�u�t�@�C����(const char* pArchiveFileName),
// �@�@�@�@�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool StorageManager::LoadImageFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	if(HasKeyTexture(pFileName)){ return false; }	// ���ɃL�[���o�^����Ă���ΏI��

	Texture* pTex = new Texture;
	bool bRes = pTex->LoadImageFromArchiveFile(pArchiveFileName, pFileName);

	if(bRes){
		umTexturePtr_.insert(pair<const char*, Texture*>{ pFileName, pTex });
	} else{
		SafeDelete(pTex);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// �摜�̎擾
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F�e�N�X�`���[
//------------------------------------------------------------------------------
Texture& StorageManager::GetTexture(const char * pFileName)
{
	if(!HasKeyTexture(pFileName)
		|| !umTexturePtr_.size()){
		SetDebugMessage("TexturLoadError! [%s] �̓X�g���[�W�ɑ��݂��܂���B�t�@�C������ǂݍ��݂܂��B\n", pFileName);
		if(!LoadImageFromFile(pFileName)){
			return s_NullTexture_;
		}
	}

	Texture* pTex = umTexturePtr_[pFileName];

	if(!pTex){ return s_NullTexture_; }

	return *pTex;	// �e�N�X�`���[�C���^�[�t�F�C�X�̃|�C���^�[��Ԃ�
}

//------------------------------------------------------------------------------
// �w�肵���t�@�C�����Ɗ֘A�t����ꂽ�摜�����݂��邩
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F���݂��邩
//------------------------------------------------------------------------------
bool StorageManager::HasKeyTexture(const char * pFileName)
{
	if(!umTexturePtr_.size()){ return false; }
	return (umTexturePtr_.find(pFileName) != umTexturePtr_.end());	// ����
}

//------------------------------------------------------------------------------
// wave�t�@�C���̓ǂݍ���
// �����@�F�t�@�C����(const char* pFileName),�T�E���h��(unsigned int soundSize)
// �߂�l�F����
//------------------------------------------------------------------------------
bool StorageManager::LoadWaveFromFile(const char* pFileName, unsigned int soundSize)
{
	if(HasKeyWave(pFileName)
		|| soundSize == 0){ return false; }
	
	umSoundSize_.insert(pair<const char*, unsigned int >{ pFileName, soundSize });
	umSoundIndex_.insert(pair<const char*, unsigned int >{ pFileName, 0 });

	bool bRes;
	Sound* pSnd = new Sound[soundSize];
	for(unsigned int i = 0U; i < soundSize; ++i){
		bRes = pSnd[i].LoadWaveFromFile(pFileName);
	}

	if(bRes){
		umSoundPtr_.insert(pair<const char*, Sound*>{ pFileName, pSnd });
	} else{
		SafeDeleteArray(pSnd);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// �A�[�J�C�u�t�@�C������wave�t�@�C����ǂݍ���
// �����@�F�A�[�J�C�u�t�@�C����(const char* pArchiveFileName),
// �@�@�@�@�t�@�C����(const char* pFileName),�T�E���h��(unsigned int soundNum)
// �߂�l�F����
//------------------------------------------------------------------------------
bool StorageManager::LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize)
{
	if(HasKeyWave(pFileName)
		|| soundSize == 0){ return false; }

	umSoundSize_.insert(pair<const char*, unsigned int >{ pFileName, soundSize });
	umSoundIndex_.insert(pair<const char*, unsigned int >{ pFileName, 0 });

	bool bRes;
	Sound* pSnd = new Sound[soundSize];
	for(unsigned int i = 0U; i < soundSize; ++i){
		bRes = pSnd[i].LoadWaveFromArchiveFile(pArchiveFileName, pFileName);
	}

	if(bRes){
		umSoundPtr_.insert(pair<const char*, Sound*>{ pFileName, pSnd });
	} else{
		SafeDeleteArray(pSnd);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// �T�E���h�̎擾
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F�T�E���h
//------------------------------------------------------------------------------
Sound& StorageManager::GetSound(const char * pFileName)
{
	if(!HasKeyWave(pFileName)
		|| !umSoundPtr_.size()){
		SetDebugMessage("WaveLoadError! [%s] �̓X�g���[�W�ɑ��݂��܂���B�t�@�C������ǂݍ��݂܂��B\n", pFileName);
		if(!LoadWaveFromFile(pFileName)){
			return s_NullSound_;
		}
	}

	Sound* pSnd = umSoundPtr_[pFileName];
	if(!pSnd){ return s_NullSound_; }

	Sound& snd = pSnd[umSoundIndex_[pFileName] % umSoundSize_[pFileName]];
	umSoundIndex_[pFileName] = umSoundIndex_[pFileName] + 1;

	return snd;
}

//------------------------------------------------------------------------------
// �w�肵���t�@�C�����Ɗ֘A�t����ꂽwave�f�[�^�����݂��邩
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F���݂��邩
//------------------------------------------------------------------------------
bool StorageManager::HasKeyWave(const char * pFileName)
{
	if(!umSoundPtr_.size()){ return false; }
	return (umSoundPtr_.find(pFileName) != umSoundPtr_.end());	// ����
}


void StorageManager::SetUnLoadImageRequest(const char * pFileName)
{
	vector<const char*>::iterator it = vecUnLoadImageList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadImageList_.end();

	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadImageList_.push_back(pFileName);
}

void StorageManager::UnLoadImage()
{
	if(umTexturePtr_.size()){
		// �C�e���[�^���g���ĉ��
		vector<const char*>::iterator it = vecUnLoadImageList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadImageList_.end();
		unordered_map<const char*, Texture*>::iterator itTexEnd = umTexturePtr_.end();
		for(; it != itEnd; ++it){
			if(umTexturePtr_.find(*it) != itTexEnd){
				unordered_map<const char*, Texture*>::iterator itTex = umTexturePtr_.find(*it);
				if(itTex->second){
					itTex->second->UnLoad();
					delete itTex->second;
					itTex->second = nullptr;
				}
				umTexturePtr_.erase(itTex);
			}
		}
	}
	vector<const char*>().swap(vecUnLoadImageList_);
}

void StorageManager::SetUnLoadWaveRequest(const char * pFileName)
{
	vector<const char*>::iterator it = vecUnLoadWaveList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadWaveList_.end();
	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadWaveList_.push_back(pFileName);
}

void StorageManager::UnLoadWave()
{
	if(umSoundPtr_.size()){
		// �C�e���[�^���g���ĉ��
		vector<const char*>::iterator it = vecUnLoadWaveList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadWaveList_.end();
		unordered_map<const char*, Sound*>::iterator itSndEnd = umSoundPtr_.end();
		for(; it != itEnd; ++it){
			if(umSoundPtr_.find(*it) != itSndEnd){
				unordered_map<const char*, Sound*>::iterator itSnd = umSoundPtr_.find(*it);
				if(itSnd->second){
					for(unsigned int i = 0U; i < umSoundSize_[itSnd->first]; ++i){
						itSnd->second[i].UnLoad();
					}
					delete[] itSnd->second;
					itSnd->second = nullptr;
				}
				umSoundPtr_.erase(itSnd);
			}
		}
	}

	vector<const char*>().swap(vecUnLoadWaveList_);
}

//------------------------------------------------------------------------------
// �S�Ẳ摜�ƃT�E���h�̔j��
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void StorageManager::UnLoadAll()
{
	// �C�e���[�^���g���ĉ��
	if(umTexturePtr_.size()){
		unordered_map<const char*, Texture*>::iterator itTex = umTexturePtr_.begin();
		unordered_map<const char*, Texture*>::iterator itTexEnd = umTexturePtr_.end();

		for(; itTex != itTexEnd; ++itTex){
			if(itTex->second){
				itTex->second->UnLoad();
				delete itTex->second;
				itTex->second = nullptr;
			}
		}
	}

	unordered_map<const char*, Texture*>().swap(umTexturePtr_);

	if(umSoundPtr_.size()){
		unordered_map<const char*, Sound*>::iterator itSnd = umSoundPtr_.begin();
		unordered_map<const char*, Sound*>::iterator itSndEnd = umSoundPtr_.end();

		for(; itSnd != itSndEnd; ++itSnd){
			if(itSnd->second){
				for(unsigned int i = 0U; i < umSoundSize_[itSnd->first]; ++i){
					itSnd->second[i].UnLoad();
				}
				delete[] itSnd->second;
				itSnd->second = nullptr;
			}
		}
	}

	unordered_map<const char*, Sound*>().swap(umSoundPtr_);
	unordered_map<const char*, unsigned int>().swap(umSoundSize_);
	unordered_map<const char*, unsigned int>().swap(umSoundIndex_);

	vector<const char*>().swap(vecUnLoadImageList_);
	vector<const char*>().swap(vecUnLoadWaveList_);

	bUnLoadAll_ = false;
}