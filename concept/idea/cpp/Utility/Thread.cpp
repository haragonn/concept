/*==============================================================================
	[Thread.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Utility/Thread.h"

using namespace std;

DWORD WINAPI Thread::ThreadCallBack(void* vp)
{
	Thread* pThread = (Thread*)vp;
	pThread->Run();
	return 0;
}

Thread::Thread()
	:hThread_(NULL)
	, hEvent_(NULL)
	, fStop_(false)
{
	hEvent_ = CreateEvent(NULL, FALSE, FALSE, NULL);
}

Thread::~Thread()
{
	StopAndWait();
	CloseHandle(hEvent_);
	CloseHandle(hThread_);
}

void Thread::Start()
{
	if(NULL != hThread_ || fStop_)
	{
		return;
	}

	hThread_ = CreateThread(NULL, 0, ThreadCallBack, (void*)this, 0, NULL);
}

void Thread::Stop()
{
	fStop_ = true;
	SetEvent(hEvent_);
}

void Thread::StopAndWait()
{
	Stop();
	if(NULL != hThread_)
	{
		WaitForSingleObject(hThread_, INFINITE);
	}
	return;
}

bool Thread::IsRunning()const
{
	return (WAIT_TIMEOUT == WaitForSingleObject(hThread_, 0));
}

bool Thread::IsValid()const
{
	if(NULL == hThread_ || IsRunning())
	{
		return !fStop_;
	}
	return false;
}

void Thread::Sleep(DWORD ms)
{
	if(!fStop_)
	{
		WaitForSingleObject(hEvent_, ms);
	}
}

