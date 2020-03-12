/*==============================================================================
	[SoundManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/Sound/SoundManager.h"
#include "../../h/Sound/Sound.h"
#include "../../h/Utility/ideaUtility.h"

#pragma comment(lib, "xaudio2.lib")

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
SoundManager::SoundManager() :
	pXAudio2_(nullptr),
	pMasteringVoice_(nullptr),
	bCom_(false),
	pListBegin_(nullptr),
	pListEnd_(nullptr)
{}

//------------------------------------------------------------------------------
// ������
// �����@�F�Ȃ�
// �߂�l�F����
//------------------------------------------------------------------------------
bool SoundManager::Init()
{
	// COM���C�u�����̏�����
	if(FAILED(CoInitializeEx(NULL, COINIT_MULTITHREADED))){
		bCom_ = false;
		return false;
	}
	bCom_ = true;

	// XAudio2�I�u�W�F�N�g�̍쐬
	if(FAILED(XAudio2Create(&pXAudio2_, 0))){
		CoUninitialize();
		bCom_ = false;
		return false;
	}

	// �}�X�^�[�{�C�X�̍쐬
	if(FAILED(pXAudio2_->CreateMasteringVoice(&pMasteringVoice_))){
		if(pXAudio2_){
			pXAudio2_->Release();
			pXAudio2_ = nullptr;
		}
		CoUninitialize();
		bCom_ = false;
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------
// �I������
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void SoundManager::UnInit()
{
	// �����c���Ă�Sound��P��
	if(pListBegin_){
		for(Sound* pSnd = pListBegin_; pSnd != nullptr; pSnd = pSnd->pNext_){
			pSnd->UnLoad();
		}
		pListBegin_ = nullptr;
	}

	// ���
	if(pMasteringVoice_){
		pMasteringVoice_->DestroyVoice();
		pMasteringVoice_ = nullptr;
	}

	SafeRelease(pXAudio2_);

	if(bCom_){
		CoUninitialize();
		bCom_ = false;
	}
}

//------------------------------------------------------------------------------
// �\�[�X�{�C�X�̐���
// �����@�F�\�[�X�{�C�X�\���̂̃|�C���^�[�̃|�C���^�[(IXAudio2SourceVoice** ppSourceVoice)
// �@�@�@�@�t�H�[�}�b�g�̍\���̂̃|�C���^�[(WAVEFORMATEX* pSourceFormat)
// �߂�l�F����
//------------------------------------------------------------------------------
bool SoundManager::CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, WAVEFORMATEX* pSourceFormat)
{
	if(!pXAudio2_){ return false; }	// ����������Ă��Ȃ���Ύ��s

	// �\�[�X�{�X�̐���
	if(FAILED(pXAudio2_->CreateSourceVoice(ppSourceVoice, pSourceFormat))){ return false; }

	return true;
}

void SoundManager::Register(Sound * pSound)
{
	if(!pSound || pSound->bRegistered_){ return; }	// NULL�`�F�b�N,�o�^�ς݃`�F�b�N

	if(!pListBegin_){ pListBegin_ = pSound; }	// �擪���Ȃ���ΐ擪��
	else{
		pListEnd_->pNext_ = pSound;	// ���݂̍Ō���̎��̃|�C���^�Ƃ���
		pSound->pPrev_ = pListEnd_;	// �O�̃|�C���^�Ɍ��݂̍Ō��������
	}

	pListEnd_ = pSound;				// �Ō���Ƃ���

	pSound->bRegistered_ = true;	// �o�^�ς݂�
}

void SoundManager::UnRegister(Sound * pSound)
{
	if(!pSound || !pSound->bRegistered_){ return; }	// NULL�`�F�b�N,�폜�ς݃`�F�b�N

	if(pSound == pListBegin_){	// �擪�̃|�C���^�Ȃ��
		pListBegin_ = pSound->pNext_;	// ���̃|�C���^��擪�Ƃ���
		if(pSound->pNext_){ pSound->pNext_->pPrev_ = nullptr; }	// ���̃|�C���^�����݂���Ȃ�΂��̃|�C���^�̑O�̃|�C���^��NULL�Ƃ���
	}else if(pSound == pListEnd_){	// �Ō���Ȃ��
		pListEnd_ = pSound->pPrev_;	// �O�̃|�C���^���Ō���Ƃ���
		if(pSound->pPrev_){ pSound->pPrev_->pNext_ = nullptr; }	// �Ō���̎��̃|�C���^��NULL�Ƃ���
	}else{
		pSound->pPrev_->pNext_ = pSound->pNext_;	// �O�̃|�C���^�̎��̃|�C���^���X�V����
		if(pSound->pNext_){ pSound->pNext_->pPrev_ = pSound->pPrev_; }	// ���̃|�C���^�����݂���Ȃ�΂��̃|�C���^�̑O�̃|�C���^���X�V����
	}

	pSound->bRegistered_ = false;	// �폜�ς݂�
}
