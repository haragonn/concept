/*==============================================================================
	[Actor2DManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/2D/Actor2DManager.h"
#include "../../h/2D/Actor2D.h"

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
Actor2DManager::Actor2DManager() :
	pListBegin_(nullptr),
	pListEnd_(nullptr)
{}

//------------------------------------------------------------------------------
// �o�^
// �����@�F�o�^����Actor2D(Actor2D* pActor2D)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Actor2DManager::Register(Actor2D* pActor2D)
{
	if(!pActor2D || pActor2D->bRegistered_){ return; }	// NULL�`�F�b�N,�o�^�ς݃`�F�b�N
	if(!pListBegin_){ pListBegin_ = pActor2D; }	// �擪���Ȃ���ΐ擪��
	else{
		pListEnd_->pNext_ = pActor2D;	// ���݂̍Ō���̎��̃|�C���^�Ƃ���
		pActor2D->pPrev_ = pListEnd_;	// �O�̃|�C���^�Ɍ��݂̍Ō��������
	}
	pListEnd_ = pActor2D;			// �Ō���Ƃ���
	pActor2D->bRegistered_ = true;	// �o�^�ς݂�
}

//------------------------------------------------------------------------------
// �o�^����
// �����@�F�o�^��������Actor2D(Actor2D * pActor2D)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Actor2DManager::UnRegister(Actor2D* pActor2D)
{
	if(!pActor2D || !pActor2D->bRegistered_){ return; }	// NULL�`�F�b�N,�폜�ς݃`�F�b�N
	if(pActor2D == pListBegin_){	// �擪�̃|�C���^�Ȃ��
		pListBegin_ = pActor2D->pNext_;	// ���̃|�C���^��擪�Ƃ���
		if(pActor2D->pNext_){ pActor2D->pNext_->pPrev_ = nullptr; }	// ���̃|�C���^�����݂���Ȃ�΂��̃|�C���^�̑O�̃|�C���^��NULL�Ƃ���
	} else if(pActor2D == pListEnd_){	// �Ō���Ȃ��
		pListEnd_ = pActor2D->pPrev_;	// �O�̃|�C���^���Ō���Ƃ���
		if(pActor2D->pPrev_){ pActor2D->pPrev_->pNext_ = nullptr; }	// �Ō���̎��̃|�C���^��NULL�Ƃ���
	} else{
		pActor2D->pPrev_->pNext_ = pActor2D->pNext_;	// �O�̃|�C���^�̎��̃|�C���^���X�V����
		if(pActor2D->pNext_){ pActor2D->pNext_->pPrev_ = pActor2D->pPrev_; }	// ���̃|�C���^�����݂���Ȃ�΂��̃|�C���^�̑O�̃|�C���^���X�V����
	}
	pActor2D->bRegistered_ = false;	// �폜�ς݂�
}

//------------------------------------------------------------------------------
// ���W�̕ێ�
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Actor2DManager::WriteDownPosition()
{
	if(pListBegin_){	// �Ǘ����Ă�����
		// ���X�g��Actor2D�ɍ��W�̕ێ���������
		for(Actor2D* pAct = pListBegin_; pAct != nullptr; pAct = pAct->pNext_){
			pAct->WriteDownPosition();
		}
	}
}
