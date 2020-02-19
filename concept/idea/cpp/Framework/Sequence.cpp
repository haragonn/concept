/*==============================================================================
	[Sequence.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/Framework/Sequence.h"
#include "../../h/Framework/Scene.h"
#include "../../h/Framework/AsyncLoadScene.h"
#include "../../h/Storage/StorageManager.h"
#include "../../../eidos/h/eidosStorage/eidosStorageManager.h"
#include "../../h/Network/NetworkManager.h"

#include <exception>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//------------------------------------------------------------------------------
// �񓯊��ǂݍ��݃X���b�h
//------------------------------------------------------------------------------
DWORD WINAPI AsyncLoadThread(void* vp)
{
	Sequence* psq = (Sequence*)vp;

	psq->AsyncLoad();
	psq->bLoadCompleted_ = true;

	ExitThread(0);
}

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
Sequence::Sequence() :
	pScene_(nullptr),
	hAsyncLoadThread_(nullptr),
	bLoadCompleted_(false){}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
Sequence::~Sequence()
{
	UnInit();
}

//------------------------------------------------------------------------------
// ������
// �����@�F�J�n����V�[���̃|�C���^�[(Scene* pInitScene)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Sequence::Init(Scene* pInitScene)
{
	if(!pInitScene){ return false; }	// NULL�`�F�b�N

	if(!pScene_){	// ���ɃV�[���������Ă��Ȃ����
		pScene_ = pInitScene;	// �V�[����o�^
		pScene_->Init();

		return true;
	}

	// �V�[���̔j��
	delete pInitScene;

	return false;
}

//------------------------------------------------------------------------------
// �I������
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Sequence::UnInit()
{
	// �X���b�h�̔j��
	if(hAsyncLoadThread_){
#ifdef _DEBUG
		// �f�o�b�O�Ȃ�3����������ŎE�����Ⴆ
		if(WaitForSingleObject(hAsyncLoadThread_, 3 * 60 * 1000) == WAIT_TIMEOUT){
			MessageBox(NULL, "AsyncLoadThread���I�����܂���B�����I�����܂��B", NULL, NULL);
			TerminateThread(hAsyncLoadThread_ , FALSE);
		}
#else
		WaitForSingleObject(hAsyncLoadThread_, INFINITE);
#endif
		CloseHandle(hAsyncLoadThread_);
		hAsyncLoadThread_ = nullptr;
	}

	// �V�[���̔j��
	if(pScene_){
		pScene_->UnInit();
		delete pScene_;
		pScene_ = nullptr;
	}
}

//------------------------------------------------------------------------------
// �X�V
// �����@�F�Ȃ�
// �߂�l�F�A�v���P�[�V�����𑱍s���邩�ǂ���
//------------------------------------------------------------------------------
int Sequence::Update(bool bRedy)
{
	if(pScene_){	// NULL�`�F�b�N
		if(pScene_->bAsync_){
			if(!bLoadCompleted_ && !hAsyncLoadThread_){
				hAsyncLoadThread_ = CreateThread(NULL, 0, AsyncLoadThread, this, 0, NULL);	// �񓯊��ǂݍ��݃X���b�h�̍쐬
			} else{
				// �񓯊��ǂݍ��݃X���b�h�̊Ď�
				DWORD ExitCode;
				GetExitCodeThread(hAsyncLoadThread_, &ExitCode);
				if(ExitCode != STILL_ACTIVE){
					CloseHandle(hAsyncLoadThread_);
					hAsyncLoadThread_ = nullptr;
				}
			}
		} else{ bLoadCompleted_ = true; }

		Scene* pNext = pScene_->Update();	// �V�[���̃A�b�v�f�[�g���玟�̃V�[�����󂯎��

		NetworkManager::Instance().WritingEnd();	// �l�b�g���[�N��������

		if(pNext == pScene_){ return 0; }	// �ς��Ȃ���΂��̂܂�

		// �񓯊��ǂݍ��݃X���b�h�̔j��
		if(hAsyncLoadThread_){
			if(WaitForSingleObject(hAsyncLoadThread_, INFINITE) == WAIT_TIMEOUT){ throw std::exception(); }
			CloseHandle(hAsyncLoadThread_);
			hAsyncLoadThread_ = nullptr;
		}

		// �قȂ�V�[�����󂯎������O�̃V�[����j�����ăV�[����؂�ւ���
		pScene_->UnInit();

		NetworkManager::Instance().WritingEnd();

		delete pScene_;

		// �X�g���[�W�̔j���v���`�F�b�N
		if(StorageManager::Instance().IsUnLoadAllRequest()){
			StorageManager::Instance().UnLoadAll();
		} else{
			if(StorageManager::Instance().GetUnLoadImageRequestSize()){
				StorageManager::Instance().UnLoadImage();
			}
			if(StorageManager::Instance().GetUnLoadWaveRequestSize()){
				StorageManager::Instance().UnLoadWave();
			}
		}

		if(eidosStorageManager::Instance().IsUnLoadAllRequest()){
			eidosStorageManager::Instance().UnLoadAll();
		} else{
			if(eidosStorageManager::Instance().GetUnLoadObjModelRequestSize()){
				eidosStorageManager::Instance().UnLoadObjModel();
			}
			if(eidosStorageManager::Instance().GetUnLoadPmdModelRequestSize()){
				eidosStorageManager::Instance().UnLoadPmdModel();
			}
		}

		// �V�[���J��
		if(pNext && bRedy){	// ���̃V�[���������
			pScene_ = pNext;

			if(pScene_->bAsync_){ bLoadCompleted_ = false; }

			pScene_->Init();

			return 1;	// ���s
		}
		pScene_ = nullptr;
	}

	return -1;	// ���f
}

//------------------------------------------------------------------------------
// �`��
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Sequence::Draw()
{
	if(pScene_){ pScene_->Draw(); }	// NULL�`�F�b�N
}

//------------------------------------------------------------------------------
// �񓯊��ǂݍ���
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Sequence::AsyncLoad()
{
	if(pScene_ && pScene_->bAsync_ && dynamic_cast<AsyncLoadScene*>(pScene_)->bLoadCompleted_){ return; }
	if(pScene_ && pScene_->bAsync_){ dynamic_cast<AsyncLoadScene*>(pScene_)->Load(); }
	if(pScene_ && pScene_->bAsync_){ dynamic_cast<AsyncLoadScene*>(pScene_)->bLoadCompleted_ = true; }
}