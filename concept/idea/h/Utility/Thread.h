/*==============================================================================
[Thread.h]
Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_THREAD_H
#define INCLUDE_IDEA_THREAD_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>

class Thread
{
public:
	Thread();
	virtual ~Thread();

	virtual void Start();			// �X���b�h�̋N��
	virtual void Stop();			// �X���b�h����~����悤����
	virtual void StopAndWait();		// �X���b�h����~����悤���߂��A���ۂɒ�~����܂ő҂�
	virtual bool IsRunning()const;	// �X���b�h���N�����Ă����Ԃ��ǂ������擾
	virtual bool IsValid()const;	// �X���b�h���L����Ԃ��ǂ������擾
	virtual void Run() = 0;			// �X���b�h�ɁA���ۂɏ�����������e

protected:
	HANDLE	hThread_;
	HANDLE	hEvent_;
	std::atomic<bool> fStop_;

	static DWORD WINAPI ThreadCallBack(void* vp);

	virtual void Sleep(DWORD milliseconds);
};

#endif	// #ifndef INCLUDE_IDEA_THREAD_H
