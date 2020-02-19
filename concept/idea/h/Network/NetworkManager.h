/*==============================================================================
	[NetworkManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_NETWORKMANAGER_H
#define INCLUDE_IDEA_NETWORKMANAGER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

//------------------------------------------------------------------------------
// 構造体定義
//------------------------------------------------------------------------------
#pragma pack(push,1)
struct InputData{	// 入力情報
	short keyData;	// キーの入力
	short lAxisX;	// 左アナログスティックのX軸
	short lAxisY;	// 左アナログスティックのY軸
	short rAxisX;	// 右アナログスティックのX軸
	short rAxisY;	// 右アナログスティックのY軸
	short lTrigger;	// 左トリガー
	short rTrigger;	// 右トリガー
};
#pragma pack(pop)

#pragma pack(push,1)
struct PacketData{	// パケット情報
	char PlayerNum;	// プレイヤー番号
	unsigned long long Cnt;
	unsigned long long Sequence;
	unsigned long long Serial;
	unsigned long  Seed;
	unsigned int Frequency;
	union{
		char cw[8];		// char型
		short sw[4];	// short型
		__int32 iw[2];	// int型
		float fw[2];	// float型
		double dw;		// double型
	}Value[100];		// 値のデータ
	InputData id[30];	// 過去30フレーム分の入力データ
};
#pragma pack(pop)

//------------------------------------------------------------------------------
// クラス名　：NetworkManager
// クラス概要：シングルトン
// 　　　　　　UDP通信の管理を行うクラス
//------------------------------------------------------------------------------
class NetworkManager{
public:
	static NetworkManager& Instance()	// 唯一のインスタンスを返す
	{
		static NetworkManager s_Instance;
		return s_Instance;
	}
	bool Init();					// 初期化
	bool SetUpServer(u_short portNum);	// サーバーの立ち上げ
	bool SetUpClient(u_short IPAddrA, u_short IPAddrB, u_short IPAddrC, u_short IPAddrD, u_short portNum);	// クライアントの立ち上げ
	void Close();	// 通信切断
	void UnInit();	// 終了処理
	bool IsSetUp(){ return bSetUp_; }		// 通信の立ち上げが完了しているかどうか
	bool IsServer(){ return bServer_; }	// サーバーかどうか
	bool IsConnect(){ return bConnect_; }	// クライアントかどうか
	bool IsCommunication(){ return bCommunication_; }	// 通信中かどうか
	void Flash();	// 送受信データの消去
	struct PacketData* GetSendData(){ return &sendBuffer_; }		// 送るデータのポインタの取得
	struct PacketData* GetReceiveData(){ return &receiveBuffer_; }	// 受け取るデータのポインタの取得
	unsigned long GetSeed();
	UINT GetFrequency(){ return frequency_ / 3; }	// 通信頻度の取得
	void WritingBegin(){ bWrighting_ = true; }	// 書き込みフラグを立てる
	void WritingEnd(){ bWrighting_ = false; }	// 書き込みフラグを下す
	bool GetSkipFlag(){ return bSkip_; }
	void SetSkipFlag(bool bSkip){ bSkip_ = bSkip; }

private:
	bool bNetwork_;					// 接続許可
	SOCKET srcSocket_;				// 自分のソケット
	SOCKET dstSocket_;				// 相手のソケット
	struct sockaddr_in srcAddr_;	// 自分のアドレス
	struct sockaddr_in dstAddr_;	// 相手のアドレス
	int dstAddrSize_;				// 相手のアドレスのサイズ
	fd_set readFds_;				// ファイルディスクリプタのリスト
	struct timeval tvConnect_;		// 接続待機時間
	struct timeval tv_;			// 通信待機時間
	bool bSetUp_;					// 立ち上げ完了フラグ
	bool bServer_;					// サーバーフラグ
	bool bConnect_;				// 接続フラグ
	bool bCommunication_;			// 通信中フラグ
	bool bWrighting_;				// 書き込みフラグ
	HANDLE hServerSendThread_;			// サーバスレッドハンドル
	HANDLE hServerReceiveThread_;			// サーバスレッドハンドル
	HANDLE hClientSendThread_;		// クライアント送信スレッドハンドル
	HANDLE hClientReceiveThread_;	// クライアント受信スレッドハンドル
	CRITICAL_SECTION cs_;			// クリティカルセクション
	CRITICAL_SECTION cs2_;			///
	struct PacketData sendBuffer_;		// 送信データ
	struct PacketData tempReceiveBuffer_;	// 一時受信データ
	struct PacketData receiveBuffer_;	// 受信データ
	unsigned long seed_;
	int frequency_;	// 通信頻度
	bool bSkip_;
	unsigned long wait_;
	
	friend UINT __stdcall ServerSendThread(void* vp);		// サーバー送信スレッド
	friend UINT __stdcall ServerReceiveThread(void* vp);	// サーバー受信スレッド
	friend UINT __stdcall ClientSendThread(void* vp);		// クライアント送信スレッド
	friend UINT __stdcall ClientReceiveThread(void* vp);	// クライアント受信スレッド
	
	void ServerSend();			// サーバー送信
	void ServerReceive();			// サーバー受信
	void ClientSend();		// クライアント送信
	void ClientReceive();	// クライアント受信

	NetworkManager();					// コンストラクタ
	~NetworkManager(){ UnInit(); }		// デストラクタ
	// コピーコンストラクタの禁止
	NetworkManager(const NetworkManager& src){}
	NetworkManager& operator=(const NetworkManager& src){}
};

#endif