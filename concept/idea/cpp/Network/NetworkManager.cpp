/*==============================================================================
[NetworkManager.cpp]
Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Network/NetworkManager.h"
#include "../../../Framework.h"
#include "../../h/Utility/Timer.h"
#include <stdio.h>
#include <process.h>
#include <exception>
#include <mmsystem.h>
#include <ctime>

#pragma comment(lib, "ws2_32.lib")

UINT __stdcall ServerSendThread(void* vp)
{
	NetworkManager* nm = (NetworkManager*)vp;
	EnterCriticalSection(&nm->cs_);
	nm->ServerSend();
	LeaveCriticalSection(&nm->cs_);
	_endthreadex(0);
	return 0;
}

UINT __stdcall ServerReceiveThread(void* vp)
{
	NetworkManager* nm = (NetworkManager*)vp;
	EnterCriticalSection(&nm->cs2_);
	nm->ServerReceive();
	LeaveCriticalSection(&nm->cs2_);
	_endthreadex(0);
	return 0;
}

UINT __stdcall ClientSendThread(void* vp)
{
	NetworkManager* nm = (NetworkManager*)vp;
	EnterCriticalSection(&nm->cs_);
	nm->ClientSend();
	LeaveCriticalSection(&nm->cs_);
	_endthreadex(0);
	return 0;
}


UINT __stdcall ClientReceiveThread(void* vp)
{
	NetworkManager* nm = (NetworkManager*)vp;
	EnterCriticalSection(&nm->cs2_);
	nm->ClientReceive();
	LeaveCriticalSection(&nm->cs2_);
	_endthreadex(0);
	return 0;
}

NetworkManager::NetworkManager()
{
	bNetwork_ = false;
	srcSocket_ = -1;
	dstSocket_ = -1;
	memset(&srcAddr_, 0, sizeof(srcAddr_));
	memset(&dstAddr_, 0, sizeof(dstAddr_));
	memset(&sendBuffer_, 0, sizeof(sendBuffer_));
	memset(&tempReceiveBuffer_, 0, sizeof(tempReceiveBuffer_));
	memset(&receiveBuffer_, 0, sizeof(receiveBuffer_));
	dstAddrSize_ = sizeof(dstAddr_);
	FD_ZERO(&readFds_);
	tvConnect_.tv_sec = 1;
	tvConnect_.tv_usec = 0;
	tv_.tv_sec = 5;
	tv_.tv_usec = 0;
	bSetUp_ = false;
	bServer_ = false;
	bConnect_ = false;
	bCommunication_ = false;
	InitializeCriticalSection(&cs_);
	InitializeCriticalSection(&cs2_);
	hServerSendThread_ = nullptr;
	hServerReceiveThread_ = nullptr;
	hClientSendThread_ = nullptr;
	hClientReceiveThread_ = nullptr;
	bWrighting_ = false;
	frequency_ = 0U;
	bSkip_ = false;
	seed_ = 0U;
	wait_ = 8U;
}

bool NetworkManager::Init()
{
	bNetwork_ = true;
	// クリティカルセクション初期化
	InitializeCriticalSection(&cs_);
	InitializeCriticalSection(&cs2_);
	WSADATA data;
	if(WSAStartup(MAKEWORD(2, 0), &data) != 0){ return false; }
	return true;
}

bool NetworkManager::SetUpServer(u_short PortNum)
{
	if(!bNetwork_ || bSetUp_){ return false; }
	Close();
	Flash();
	int res;
	// ソケットの生成
	if(srcSocket_ != -1){
		closesocket(srcSocket_);
		srcSocket_ = -1;
	}
	srcSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(dstSocket_ != -1){
		closesocket(dstSocket_);
		dstSocket_ = -1;
	}
	if(srcSocket_ < 0){ return false; }
	// ソケットのオプションの設定
	const int on = 1;
	setsockopt(srcSocket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, (socklen_t)sizeof(on));
	// sockaddr_in構造体のセット
	memset(&srcAddr_, 0, sizeof(srcAddr_));
	srcAddr_.sin_port = htons(PortNum);
	srcAddr_.sin_family = AF_INET;
	srcAddr_.sin_addr.s_addr = htonl(INADDR_ANY);
	// ソケットのバインド
	res = bind(srcSocket_, (struct sockaddr*)&srcAddr_, sizeof(srcAddr_));
	if(res < 0){
		closesocket(srcSocket_);
		return false;
	}
	// fd_setの登録
	FD_ZERO(&readFds_);
	FD_SET(srcSocket_, &readFds_);

	bSetUp_ = true;
	bServer_ = true;
	if(!hServerSendThread_){
		hServerSendThread_ = (HANDLE)_beginthreadex(NULL, 0, ServerSendThread, this, 0, NULL);
	}
	if(!hServerReceiveThread_){
		hServerReceiveThread_ = (HANDLE)_beginthreadex(NULL, 0, ServerReceiveThread, this, 0, NULL);
	}

	return true;
}
bool NetworkManager::SetUpClient(u_short IPAddrA, u_short IPAddrB, u_short IPAddrC, u_short IPAddrD, u_short PortNum)
{
	if(!bNetwork_ || bSetUp_){ return false; }
	Close();
	Flash();
	/*int res;*/
	char ipAddr[32] = "";
	sprintf_s(ipAddr, 32,"%u.%u.%u.%u", IPAddrA, IPAddrB, IPAddrC, IPAddrD);

	// ソケットの生成
	if(srcSocket_ != -1){
		closesocket(srcSocket_);
		srcSocket_ = -1;
	}
	srcSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(srcSocket_ < 0){ return false; }
	// sockaddr_in 構造体のセット
	memset(&srcAddr_, 0, sizeof(srcAddr_));
	srcAddr_.sin_port = htons(PortNum);
	srcAddr_.sin_family = AF_INET;
	InetPton(srcAddr_.sin_family, ipAddr, &srcAddr_.sin_addr.s_addr);

	bSetUp_ = true;
	bServer_ = false;
	if(!hClientSendThread_){
		hClientSendThread_ = (HANDLE)_beginthreadex(NULL, 0, ClientSendThread, this, 0, NULL);
	}
	if(!hClientReceiveThread_){
		hClientReceiveThread_ = (HANDLE)_beginthreadex(NULL, 0, ClientReceiveThread, this, 0, NULL);
	}
	return true;
}

void NetworkManager::Flash()
{
	unsigned long long tsc = sendBuffer_.Cnt, trc = receiveBuffer_.Cnt;
	unsigned long tss = sendBuffer_.Seed, trs = receiveBuffer_.Seed;
	memset(&sendBuffer_, 0, sizeof(sendBuffer_));
	memset(&tempReceiveBuffer_, 0, sizeof(tempReceiveBuffer_));
	memset(&receiveBuffer_, 0, sizeof(receiveBuffer_));
	sendBuffer_.Cnt = tsc;
	receiveBuffer_.Cnt = trc;
	tempReceiveBuffer_.Cnt = trc;
	sendBuffer_.Seed = tss;
	receiveBuffer_.Seed = trs;
	tempReceiveBuffer_.Seed = trs;
	sendBuffer_.Frequency = 180U;
	receiveBuffer_.Frequency = 180U;
	tempReceiveBuffer_.Frequency = 180U;
}

void NetworkManager::Close()
{
	bSetUp_ = false;	// 準備フラグをオフに
	bServer_ = false;
	frequency_ = 0U;
	memset(&sendBuffer_, 0, sizeof(sendBuffer_));
	memset(&tempReceiveBuffer_, 0, sizeof(tempReceiveBuffer_));
	memset(&receiveBuffer_, 0, sizeof(receiveBuffer_));
	if(hServerSendThread_){
		if(WaitForSingleObject(hServerSendThread_, INFINITE) == WAIT_TIMEOUT){ throw std::exception(); }
		CloseHandle(hServerSendThread_);
		hServerSendThread_ = nullptr;
	}
	if(hServerReceiveThread_){
		if(WaitForSingleObject(hServerReceiveThread_, INFINITE) == WAIT_TIMEOUT){ throw std::exception(); }
		CloseHandle(hServerReceiveThread_);
		hServerReceiveThread_ = nullptr;
	}
	if(hClientSendThread_){
		if(WaitForSingleObject(hClientSendThread_, INFINITE) == WAIT_TIMEOUT){ throw std::exception(); }
		CloseHandle(hClientSendThread_);
		hClientSendThread_ = nullptr;
	}
	if(hClientReceiveThread_){
		if(WaitForSingleObject(hClientReceiveThread_, INFINITE) == WAIT_TIMEOUT){ throw std::exception(); }
		CloseHandle(hClientReceiveThread_);
		hClientReceiveThread_ = nullptr;
	}
}

void NetworkManager::UnInit()
{
	Close();
	WSACleanup();
}

unsigned long NetworkManager::GetSeed()
{
	return ++seed_;
}

void NetworkManager::ServerSend()
{
	wait_ = 8U;
	while(bSetUp_){
		Sleep(1);
		while(bSetUp_ && bConnect_ && dstSocket_ != -1){
			++sendBuffer_.Cnt;
			Sleep(wait_);
			while(bSetUp_ && bWrighting_){ Sleep(1); }
			send(dstSocket_, (char*)&sendBuffer_, sizeof(sendBuffer_), 0);
		}
	}
	bConnect_ = false;
	bCommunication_ = false;
	bSetUp_ = false;
}

void NetworkManager::ServerReceive()
{
	int err = 0;
	int numrcv = 0;
	UINT Frequency = 0U;
	fd_set fds;
	Timer Timer;		// FPS計測用タイマー
	while(bSetUp_){
		bCommunication_ = false;
		Sleep(2);
		bCommunication_ = true;
		if(Timer.GetHighPrecisionElapsed() >= 3000.0){
			Timer.SetStartTime();
			sendBuffer_.Frequency = Frequency;
			frequency_ = Frequency / 3;
			Frequency = 0U;
			if(receiveBuffer_.Frequency < 180U){
				wait_ = max(1U, wait_ - 1U);
			}
			if(receiveBuffer_.Frequency > 200U){
				++wait_;
			}
		}
		FD_ZERO(&fds);
		FD_SET(srcSocket_, &fds);
		// 接続待機
		err = select((int)srcSocket_ + 1, &fds, NULL, NULL, (dstSocket_ == -1) ? &tvConnect_ : &tv_);
		if(err < 0){ break; }
		// 通信
		if(dstSocket_ != -1 && (FD_ISSET(srcSocket_, &fds))){
			numrcv = recv(srcSocket_, (char*)&tempReceiveBuffer_, sizeof(tempReceiveBuffer_), 0);
			if(numrcv < 0){ break; }
			else{
				if(tempReceiveBuffer_.Cnt > receiveBuffer_.Cnt){
					++Frequency;
					memcpy(&receiveBuffer_, &tempReceiveBuffer_, sizeof(receiveBuffer_));
				}
			}
		}else if(dstSocket_ != -1){ break; }
		// 新規通信接続
		if(FD_ISSET(srcSocket_, &fds)){
			if(dstSocket_ == -1){
				numrcv = recvfrom(srcSocket_, (char*)&receiveBuffer_, sizeof(receiveBuffer_), 0, (struct sockaddr*)&dstAddr_, &dstAddrSize_);
				if(numrcv ==0 || numrcv ==-1 ){ continue; }
				dstSocket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
				if(dstSocket_ < 0){
					dstSocket_ = -1;
					continue;
				}
				const int on = 1;
				setsockopt(dstSocket_, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, (socklen_t)sizeof(on));
				err = bind(dstSocket_, (struct sockaddr*)&srcAddr_, sizeof(srcAddr_));
				if(err != 0){
					closesocket(dstSocket_);
					dstSocket_ = -1;
					continue;
				}
				err = connect(dstSocket_, (struct sockaddr *)&dstAddr_, sizeof(dstAddr_));
				if (err != 0) {
					closesocket(dstSocket_);
					dstSocket_ = -1;
					continue;
				}
				if(!sendBuffer_.Seed){
					seed_ = sendBuffer_.Seed = static_cast<unsigned long>(time(NULL));
				}
				send(dstSocket_, (char*)&sendBuffer_, sizeof(sendBuffer_), 0);
				bConnect_ = true;
				Timer.SetStartTime();
			}
		}
	}
	bConnect_ = false;
	bCommunication_ = false;
	bSetUp_ = false;
	frequency_ = 0U;
	if(srcSocket_ != -1){
		closesocket(srcSocket_);
		srcSocket_ = -1;
	}
	if(dstSocket_ != -1){
		closesocket(dstSocket_);
		dstSocket_ = -1;
	}
}

void NetworkManager::ClientSend()
{
	int err = 0;
	int numrcv = 0;
	wait_ = 8U;
	fd_set fds;
	while(bSetUp_){
		Sleep(1);
		err = connect(srcSocket_, (struct sockaddr*)&srcAddr_, sizeof(srcAddr_));
		if (err != 0){ continue; }
		sendto(srcSocket_, (char*)&sendBuffer_, sizeof(sendBuffer_), 0, (struct sockaddr*)&srcAddr_, sizeof(srcAddr_));
		FD_ZERO(&fds);
		FD_SET(srcSocket_, &fds);
		// 接続待機
		err = select((int)srcSocket_ + 1, &fds, NULL, NULL, &tvConnect_);
		if(err < 0){ break; }
		if(FD_ISSET(srcSocket_, &fds)){
			numrcv = recv(srcSocket_, (char*)&receiveBuffer_, sizeof(receiveBuffer_), 0);
			if(numrcv < 0){ continue; }
			seed_ = sendBuffer_.Seed = receiveBuffer_.Seed;
			bConnect_ = true;
		}else{ continue; }
		while(bSetUp_ && bConnect_ && srcSocket_ != -1){
			++sendBuffer_.Cnt;
			Sleep(wait_);
			while(bSetUp_ && bWrighting_){ Sleep(1); }
			sendto(srcSocket_, (char*)&sendBuffer_, sizeof(sendBuffer_), 0, (struct sockaddr*)&srcAddr_, sizeof(srcAddr_));
		}
	}
	bConnect_ = false;
	bCommunication_ = false;
	bSetUp_ = false;
}

void NetworkManager::ClientReceive()
{
	int err = 0;
	int numrcv = 0;
	UINT Frequency = 0U;
	fd_set fds;
	Timer Timer;		// FPS計測用タイマー
	Timer.SetStartTime();	// タイマー開始
	while(bSetUp_){
		Sleep(1);
		while(bSetUp_ && bConnect_){
			bCommunication_ = false;
			Sleep(2);
			bCommunication_ = true;
			if(Timer.GetHighPrecisionElapsed() >= 3000.0){
				Timer.SetStartTime();
				sendBuffer_.Frequency = Frequency;
				frequency_ = Frequency / 3;
				Frequency = 0U;
				if(receiveBuffer_.Frequency < 180U){
					wait_ = max(1U, wait_ - 1U);
				}
				if(receiveBuffer_.Frequency > 200U){
					++wait_;
				}
			}
			if(srcSocket_ == -1){
				bSetUp_ = false;
				break;
			}
			FD_ZERO(&fds);
			FD_SET(srcSocket_, &fds);
			// 接続待機
			err = select((int)srcSocket_ + 1, &fds, NULL, NULL, &tv_);
			if(err < 0){
				bSetUp_ = false;
				break;
			}
			if(FD_ISSET(srcSocket_, &fds)){
				numrcv = recv(srcSocket_, (char*)&tempReceiveBuffer_, sizeof(tempReceiveBuffer_), 0);
				if(numrcv < 0){
					bSetUp_ = false;
					break;
				}
				if(tempReceiveBuffer_.Cnt > receiveBuffer_.Cnt){
					++Frequency;
					memcpy(&receiveBuffer_, &tempReceiveBuffer_, sizeof(receiveBuffer_));
				}
				bConnect_ = true;
			}else{
				bSetUp_ = false;
				break;
			}
		}
	}
	bConnect_ = false;
	bCommunication_ = false;
	bSetUp_ = false;
	if(srcSocket_ != -1){
		closesocket(srcSocket_);
		srcSocket_ = -1;
	}
	frequency_ = 0;
}