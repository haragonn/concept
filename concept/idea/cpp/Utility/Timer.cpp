/*==============================================================================
	[Timer.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/Utility/Timer.h"
#pragma comment(lib, "winmm.lib")

//------------------------------------------------------------------------------
// �ÓI�����o
//------------------------------------------------------------------------------
bool Timer::s_bSetPeriod_ = false;	// timeBeginPeriod()�����t���O

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
Timer::Timer()
{
	// �^�C�}�[�̐��x�����߂�
	if(!s_bSetPeriod_){
		timeBeginPeriod(1);
		s_bSetPeriod_ = true;
	}
	// �[���N���A
	ZeroMemory(&liStart_, sizeof(LARGE_INTEGER));
	ZeroMemory(&liNow_, sizeof(LARGE_INTEGER));
	ZeroMemory(&liFreq_, sizeof(LARGE_INTEGER));
	// ������\�p�t�H�[�}���X�J�E���^�����݂��邩�m�F
	if(QueryPerformanceCounter(&liStart_)){
		bPerformanceCounter_ = true;
		QueryPerformanceFrequency(&liFreq_);
		clock_ = 1000.0 / liFreq_.QuadPart;	// �ő啪��\����1�N���b�N�̒P�ʂ�����o��
	}else{ bPerformanceCounter_ = false; }
}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
Timer::~Timer()
{
	// �^�C�}�[�̐��x��߂�
	if(s_bSetPeriod_){
		timeEndPeriod(1);
		s_bSetPeriod_ = false;
	}
}