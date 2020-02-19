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

	virtual void Start();			// スレッドの起動
	virtual void Stop();			// スレッドが停止するよう命令
	virtual void StopAndWait();		// スレッドが停止するよう命令し、実際に停止するまで待つ
	virtual bool IsRunning()const;	// スレッドが起動している状態かどうかを取得
	virtual bool IsValid()const;	// スレッドが有効状態かどうかを取得
	virtual void Run() = 0;			// スレッドに、実際に処理させる内容

protected:
	HANDLE	hThread_;
	HANDLE	hEvent_;
	std::atomic<bool> fStop_;

	static DWORD WINAPI ThreadCallBack(void* vp);

	virtual void Sleep(DWORD milliseconds);
};

#endif	// #ifndef INCLUDE_IDEA_THREAD_H
