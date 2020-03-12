/*==============================================================================
	[SoundManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SOUNDMANAGER_H
#define INCLUDE_IDEA_SOUNDMANAGER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <XAudio2.h>

class Sound;

//------------------------------------------------------------------------------
// �N���X���@�FSoundManager
// �N���X�T�v�F�V���O���g��
// �@�@�@�@�@�@XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X�ƃ}�X�^�[�{�C�X�̊Ǘ����s��
//------------------------------------------------------------------------------
class SoundManager{
public:
	static SoundManager& Instance()	// �B��̃C���X�^���X��Ԃ�
	{
		static SoundManager s_Instance;
		return s_Instance;
	}

	bool Init();	// ������
	void UnInit();	// �I������

	bool CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, WAVEFORMATEX* pSourceFormat);	// �\�[�X�{�C�X�̐���

	void Register(Sound* pSound);		// �o�^
	void UnRegister(Sound* pSound);		// �o�^����

private:
	IXAudio2* pXAudio2_;						// XAudio2�I�u�W�F�N�g�ւ̃C���^�[�t�F�C�X

	IXAudio2MasteringVoice* pMasteringVoice_;	// �}�X�^�[�{�C�X

	bool bCom_;									// CoInitializeEx()�����t���O

	Sound* pListBegin_;	// ���X�g�̎n�܂�
	Sound* pListEnd_;	// ���X�g�̏I���

	SoundManager();					// �R���X�g���N�^
	~SoundManager(){ UnInit(); }	// �f�X�g���N�^

	// �R�s�[�R���X�g���N�^�̋֎~
	SoundManager(const SoundManager& src){}
	SoundManager& operator=(const SoundManager& src){}
};

#endif // #ifndef INCLUDE_IDEA_SOUNDMANAGER_H