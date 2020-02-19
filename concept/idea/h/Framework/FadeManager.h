/*==============================================================================
	[FadeManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FADEMANAGER_H
#define INCLUDE_IDEA_FADEMANAGER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../2D/Sprite.h"

//------------------------------------------------------------------------------
// �N���X���@�FFadeManager
// �N���X�T�v�F�V���O���g��
// �@�@�@�@�@�@�t�F�[�h�̐ݒ�,�`����s��
//------------------------------------------------------------------------------
class FadeManager{
public:
	static FadeManager& Instance()	// �B��̃C���X�^���X��Ԃ�
	{
		static FadeManager s_Instance;
		return s_Instance;
	}

	void Init(float width, float height);	// ������
	void UnInit();	// �I������

	void Update();	// �X�V

	void SetFade(int time, float r = 1.0f, float g = 1.0f, float b = 1.0f);	// �t�F�[�h�̐ݒ�
	bool IsFading()const{ return !!(int)(a_ + 0.999999f); }					// �t�F�[�h�����ǂ���
	bool IsFadeOutCompleted()const{ return bFadeOutCompleted_; }			// �t�F�[�h�A�E�g�������������ǂ���

	void Draw();	// �`��

private:
	Sprite spr_;	// �X�v���C�g

	float r_;		// �Ԑ���
	float g_;		// �ΐ���
	float b_;		// ����
	float a_;		// a�l

	float value_;	// �ω���

	bool bFadeEnable_;			// �L�����ǂ���
	bool bFadeOut_;				// �t�F�[�h�A�E�g�t���O
	bool bFadeOutCompleted_;	// �t�F�[�h�A�E�g�����t���O

	FadeManager() :	// �R���X�g���N�^
		r_(1.0f), g_(1.0f), b_(1.0f), a_(0.0f), value_(1.0f),
		bFadeEnable_(false), bFadeOut_(false), bFadeOutCompleted_(false){}

	// �R�s�[�R���X�g���N�^�̋֎~
	FadeManager(const FadeManager& src){}
	FadeManager& operator =(const FadeManager& src){}
};

#endif // #ifndef INCLUDE_IDEA_FADEMANAGER_H