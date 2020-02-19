/*==============================================================================
	[Sequence.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SEQUENCE_H
#define INCLUDE_IDEA_SEQUENCE_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>

class Scene;

//------------------------------------------------------------------------------
// �N���X���@�FSequence
// �N���X�T�v�F�V���O���g��
// �@�@�@�@�@�@�V�[�P���X������s���N���X
//------------------------------------------------------------------------------
class Sequence{
public:
	static Sequence& Instance()	// �B��̃C���X�^���X��Ԃ�
	{
		static Sequence s_Instance;
		return s_Instance;
	}

	bool Init(Scene* pInitScene);	// ������
	void UnInit();	// �I������

	int Update(bool bRedy);	// �X�V

	void Draw();	// �`��

private:
	friend class Framework;

	Scene* pScene_;				// ���݂̃V�[��

	HANDLE hAsyncLoadThread_;	// �񓯊��ǂݍ��݃X���b�h�̃n���h��
	std::atomic<bool> bLoadCompleted_;		// �񓯊��ǂݍ��݂̊���

	friend DWORD WINAPI AsyncLoadThread(void* vp);
	void AsyncLoad();

	Sequence();		// �R���X�g���N�^
	~Sequence();	// �f�X�g���N�^

	// �R�s�[�R���X�g���N�^�̋֎~
	Sequence(const Sequence& src){}
	Sequence& operator=(const Sequence& src){}
};

#endif	// #ifndef INCLUDE_SEQUENCE_H