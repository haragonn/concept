/*==============================================================================
	[AsyncLoadScene.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ASYNCLOADSCENE_H
#define INCLUDE_IDEA_ASYNCLOADSCENE_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Scene.h"
#include <atomic>

//------------------------------------------------------------------------------
// �N���X���@�FAsyncScene
// �N���X�T�v�F�񓯊��ǂݍ��ݕt���̃Q�[���V�[���N���X�̊��ƂȂ�N���X
//------------------------------------------------------------------------------
class AsyncLoadScene : public Scene{
public:
	AsyncLoadScene() : bLoadCompleted_(false){ bAsync_ = true; }	// �R���X�g���N�^
	virtual ~AsyncLoadScene(){}		// �f�X�g���N�^

	virtual void Load() = 0;		// �񓯊��ǂݍ���

	bool IsLoadCompleted(){ return bLoadCompleted_; }	// �񓯊��ǂݍ��݂������������ǂ���

private:
	friend class Sequence;

	std::atomic<bool> bLoadCompleted_;	// �񓯊��ǂݍ��݊����t���O
};

#endif	// #ifndef INCLUDE_IDEA_ASYNCLOADSCENE_H