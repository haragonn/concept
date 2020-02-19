/*==============================================================================
	[Fade.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FADE_H
#define INCLUDE_IDEA_FADE_H

#include "../Utility/ideaColor.h"

//------------------------------------------------------------------------------
// �N���X���@�FFade
// �N���X�T�v�F�t�F�[�h�̊Ǘ����s���N���X
//------------------------------------------------------------------------------
class Fade{
public:
	static void SetFade(int time, Color color);	// �t�F�[�h�̐ݒ�
	static void SetFade(int time, float r = 1.0f, float g = 1.0f, float b = 1.0f);	// �t�F�[�h�̐ݒ�
	static bool IsFading();						// �t�F�[�h�A�E�g�����ǂ���
	static bool IsFadeOutCompleted();				// �t�F�[�h�A�E�g�������������ǂ���
};

#endif	// #ifndef INCLUDE_IDEA_FADE_H