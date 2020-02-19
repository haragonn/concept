/*==============================================================================
	[Sound.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLIDE_IDEA_SOUND_H
#define INCLIDE_IDEA_SOUND_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct IXAudio2SourceVoice;

//------------------------------------------------------------------------------
// �N���X���@�FSound
// �N���X�T�v�Fwave�t�@�C���̓ǂݍ���,�\�[�X�{�C�X�̊Ǘ�,�Đ����s��
//------------------------------------------------------------------------------
class Sound{
public:
	Sound();	// �R���X�g���N�^
	~Sound();	// �f�X�g���N�^
	bool LoadWaveFromFile(const char* pFileName);		// wave�t�@�C���̓ǂݍ���
	bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName);		// �A�[�J�C�u�t�@�C������wave�t�@�C����ǂݍ���
	bool LoadWaveFromStorage(const char* pFileName);	// �X�g���[�W����wave�t�@�C����ǂݍ���
	void UnLoad();	// �\�[�X�{�C�X�̔j��
	void Play(bool bLoop = false, bool bCueing = true, UINT32 begin = 0);	// ���y�̍Đ�
	void Stop();	// ���y�̒�~
	void Cueing();	// ���y�̓��o��
	void SetVolume(float volume = 1.0f);	// ���ʂ̐ݒ�
	bool IsPlaying();	// �Đ�����
	bool IsLooping();	// ���[�v���Ă��邩

private:
	friend class SoundManager;

	IXAudio2SourceVoice* pSourceVoice_;	// �\�[�X�{�C�X
	BYTE* pDataAudio_;	// �I�[�f�B�I���
	DWORD audioSize_;	// �I�[�f�B�I�T�C�Y
	bool bPlay_;		// �Đ�����
	bool bLoop_;		// ���[�v���Ă��邩
	bool bStorage_;		// �X�g���[�W�g�p�t���O
	bool bRegistered_;	// �o�^�t���O
	Sound* pPrev_;		// �O�̃m�[�h
	Sound* pNext_;		// ���̃m�[�h
	
	// �R�s�[�R���X�g���N�^�̋֎~
	Sound(const Sound& src){}
	Sound& operator=(const Sound& src){}
};

#endif // #ifndef INCLIDE_IDEA_SOUND_H