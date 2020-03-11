/*==============================================================================
	[Actor2DManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_ACTOR2DMANAGER_H
#define INCLUDE_ACTOR2DMANAGER_H

class Actor2D;

//------------------------------------------------------------------------------
// �N���X���@�FActor2DManager
// �N���X�T�v�F�V���O���g��
// �@�@�@�@�@�@Actor2D�̊Ǘ����s��
//------------------------------------------------------------------------------
class Actor2DManager{
public:
	// �B��̃C���X�^���X
	static Actor2DManager& Instance()
	{
		static Actor2DManager s_Instance;
		return s_Instance;
	}

	// �o�^
	void Register(Actor2D* pActor2D);

	// �o�^����
	void UnRegister(Actor2D* pActor2D);

	// ���W�̕ێ�
	void WriteDownPosition();

private:
	Actor2D* pListBegin_;	// ���X�g�̎n�܂�
	Actor2D* pListEnd_;		// ���X�g�̏I���

	// �����̋֎~
	Actor2DManager();
	~Actor2DManager(){}

	// �R�s�[�R���X�g���N�^�̋֎~
	Actor2DManager(const Actor2DManager& src){}
	Actor2DManager& operator=(const Actor2DManager& src){}
};

#endif	// #ifndef INCLUDE_ACTOR2DMANAGER_H