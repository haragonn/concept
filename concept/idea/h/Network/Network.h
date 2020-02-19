/*==============================================================================
	[Network.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_NETWORK_H
#define INCLUDE_IDEA_NETWORK_H

//------------------------------------------------------------------------------
// 定数定義
//------------------------------------------------------------------------------
const unsigned int WAIT_INFINITY = (unsigned int)-1;	// 無限に待機

//------------------------------------------------------------------------------
// クラス名　：Network
// クラス概要：UDP通信を仲介するクラス
// 　　　　　　送受信情報の更新,管理を行う
//------------------------------------------------------------------------------
class Network{
public:
	Network();	// コンストラクタ
	bool SetUpServer(unsigned short portNum);	// サーバーの立ち上げ
	bool SetUpClient(unsigned short IPAddrA, unsigned short IPAddrB, unsigned short IPAddrC, unsigned short IPAddrD, unsigned short portNum);	// クライアントの立ち上げ
	void Close();			// 通信切断
	bool IsSetUp()const;	// 通信が立ち上がっているかどうか
	bool IsServer()const;	// サーバーかどうか
	bool IsConnect()const;	// 通信がつながっているかどうか
	bool WaitConnect(unsigned int timeOut = WAIT_INFINITY);	// 通信の同期
	char GetChar(unsigned int section = 99, unsigned int key = 7)const;	// 受信データの取得
	short GetShort(unsigned int section = 99, unsigned int key = 3)const;	///
	int GetInt(unsigned int section = 99, unsigned int key = 1)const;		///
	float GetFloat(unsigned int section = 99, unsigned int key = 1)const;	///
	double GetDouble(unsigned int section = 99)const;						///
	bool SetChar(unsigned int section = 99, unsigned int key = 7, char Value = 0);			// 送信データの設定
	bool SetShort(unsigned int section = 99, unsigned int key = 3, short Value = 0);		///
	bool SetInt(unsigned int section = 99, unsigned int key = 1, int Value = 0);			///
	bool SetFloat(unsigned int section = 99, unsigned int key = 1, float Value = 0.0f);		///
	bool SetDouble(unsigned int section = 99, double Value = 0.0);							///
	void Flash();						// 送受信データの消去
	unsigned long GetSeed();			// シード値の取得
	unsigned int GetFrequency();		// 通信頻度の取得
	int GetLag();						// 通信ラグの取得

private:
	unsigned long long serial_;		// データの通し番号
};

#endif	// #ifndef INCLUDE_IDEA_NETWORK_H