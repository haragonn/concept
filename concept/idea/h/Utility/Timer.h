/*==============================================================================
	[Timer.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TIMER_H
#define INCLUDE_IDEA_TIMER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mmsystem.h>

//------------------------------------------------------------------------------
// �N���X���@�FTimer
// �N���X�T�v�F���Ԍv�����s���N���X
//------------------------------------------------------------------------------
class Timer{
public:
	Timer();	// �R���X�g���N�^
	~Timer();	// �f�X�g���N�^
	void SetStartTime()	// �v���J�n�����̐ݒ�
	{
		dwStart_ = timeGetTime();
		if(bPerformanceCounter_){ QueryPerformanceCounter(&liStart_); }
	}
	unsigned long GetElapsed()	// �o�ߎ��Ԃ̎擾
	{
		return static_cast<unsigned long>(timeGetTime() - dwStart_);
	}
	double GetHighPrecisionElapsed()	// �����x�Ȍo�ߎ��Ԃ̎擾
	{
		if(bPerformanceCounter_){
			QueryPerformanceCounter(&liNow_);
			return (liNow_.QuadPart - liStart_.QuadPart) * clock_;
		}
		return static_cast<double>(timeGetTime() - dwStart_);
	}

private:
	static bool s_bSetPeriod_;	// timeBeginPeriod()�����t���O
	DWORD dwStart_;				// �J�n����
	LARGE_INTEGER liStart_, liNow_, liFreq_;	// ����
	double clock_;				// 1�N���b�N�P��
	bool bPerformanceCounter_;	// �����x�^�C�}�[�����݂��邩
};

#endif // #ifndef INCLUDE_IDEA_TIMER_H