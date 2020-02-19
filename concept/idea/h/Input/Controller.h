/*==============================================================================
	[Controller.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_CONTROLLER_H
#define INCLUDE_IDEA_CONTROLLER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../Utility/ideaMath.h"
#include <vector>

//------------------------------------------------------------------------------
// 列挙型定義
//------------------------------------------------------------------------------
enum ReplayMode{
	REPLAY_NONE,
	REPLAY_REC,
	REPLAY_PLAY,
	REPLAY_MAX
};

enum ConnectMode{
	CONNECT_NONE,
	CONNECT_SEND,
	CONNECT_RECEIVE,
	CONNECT_MAX
};

enum class KeyCode{
	KEY_CODE_UNUSED = 0x0,
	ESCAPE          = 0x01,
	_1              = 0x02,
	_2              = 0x03,
	_3              = 0x04,
	_4              = 0x05,
	_5              = 0x06,
	_6              = 0x07,
	_7              = 0x08,
	_8              = 0x09,
	_9              = 0x0A,
	_0              = 0x0B,
	MINUS           = 0x0C,
	EQUALS          = 0x0D,
	BACKSPACE       = 0x0E,
	TAB             = 0x0F,
	Q               = 0x10,
	W               = 0x11,
	E               = 0x12,
	R               = 0x13,
	T               = 0x14,
	Y               = 0x15,
	U               = 0x16,
	I               = 0x17,
	O               = 0x18,
	P               = 0x19,
	LBRACKET        = 0x1A,
	RBRACKET        = 0x1B,
	RETURN          = 0x1C,
	LCONTROL        = 0x1D,
	A               = 0x1E,
	S               = 0x1F,
	D               = 0x20,
	F               = 0x21,
	G               = 0x22,
	H               = 0x23,
	J               = 0x24,
	K               = 0x25,
	L               = 0x26,
	SEMICOLON       = 0x27,
	APOSTROPHE      = 0x28,
	GRAVE           = 0x29,
	LSHIFT          = 0x2A,
	BACKSLASH       = 0x2B,
	Z               = 0x2C,
	X               = 0x2D,
	C               = 0x2E,
	V               = 0x2F,
	B               = 0x30,
	N               = 0x31,
	M               = 0x32,
	COMMA           = 0x33,
	PERIOD          = 0x34,
	SLASH           = 0x35,
	RSHIFT          = 0x36,
	NUMPADSTAR      = 0x37,
	LALT            = 0x38,
	SPACE           = 0x39,
	CAPSLOCK        = 0x3A,
	F1              = 0x3B,
	F2              = 0x3C,
	F3              = 0x3D,
	F4              = 0x3E,
	F5              = 0x3F,
	F6              = 0x40,
	F7              = 0x41,
	F8              = 0x42,
	F9              = 0x43,
	F10             = 0x44,
	NUMLOCK         = 0x45,
	SCROLL          = 0x46,
	NUMPAD7         = 0x47,
	NUMPAD8         = 0x48,
	NUMPAD9         = 0x49,
	NUMPADMINUS     = 0x4A,
	NUMPAD4         = 0x4B,
	NUMPAD5         = 0x4C,
	NUMPAD6         = 0x4D,
	NUMPADPLUS      = 0x4E,
	NUMPAD1         = 0x4F,
	NUMPAD2         = 0x50,
	NUMPAD3         = 0x51,
	NUMPAD0         = 0x52,
	NUMPADPERIOD    = 0x53,
	OEM_102         = 0x56,
	F11             = 0x57,
	F12             = 0x58,
	F13             = 0x64,
	F14             = 0x65,
	F15             = 0x66,
	KANA            = 0x70,
	ABNT_C1         = 0x73,
	CONVERT         = 0x79,
	NOCONVERT       = 0x7B,
	YEN             = 0x7D,
	ABNT_C2         = 0x7E,
	NUMPADEQUALS    = 0x8D,
	PREVTRACK       = 0x90,
	AT              = 0x91,
	COLON           = 0x92,
	UNDERLINE       = 0x93,
	KANJI           = 0x94,
	STOP            = 0x95,
	AX              = 0x96,
	UNLABELED       = 0x97,
	NEXTTRACK       = 0x99,
	NUMPADENTER     = 0x9C,
	RCONTROL        = 0x9D,
	MUTE            = 0xA0,
	CALCULATOR      = 0xA1,
	PLAYPAUSE       = 0xA2,
	MEDIASTOP       = 0xA4,
	VOLUMEDOWN      = 0xAE,
	VOLUMEUP        = 0xB0,
	WEBHOME         = 0xB2,
	NUMPADCOMMA     = 0xB3,
	NUMPADSLASH     = 0xB5,
	SYSRQ           = 0xB7,
	RALT            = 0xB8,
	PAUSE           = 0xC5,
	HOME            = 0xC7,
	UP              = 0xC8,
	PGUP            = 0xC9,
	LEFT            = 0xCB,
	RIGHT           = 0xCD,
	END             = 0xCF,
	DOWN            = 0xD0,
	PGDN            = 0xD1,
	INSERT          = 0xD2,
	DELETEKEY       = 0xD3,
	LWIN            = 0xDB,
	RWIN            = 0xDC,
	APPS            = 0xDD,
	POWER           = 0xDE,
	SLEEP           = 0xDF,
	WAKE            = 0xE3,
	WEBSEARCH       = 0xE5,
	WEBFAVORITES    = 0xE6,
	WEBREFRESH      = 0xE7,
	WEBSTOP         = 0xE8,
	WEBFORWARD      = 0xE9,
	WEBBACK         = 0xEA,
	MYCOMPUTER      = 0xEB,
	MAIL            = 0xEC,
	MEDIASELECT     = 0xED,
	KEY_CODE_MAX    = 0xEE
};

enum PadButton{
	A = 0,
	B = 1,
	X = 2,
	Y = 3,
	L = 4,
	R = 5,
	START = 6,
	BACK = 7,
	LS = 8,
	RS = 9,
	BUTTON_MAX = 10
};

//------------------------------------------------------------------------------
// クラス名　：Controller
// クラス概要：入力情報を更新,格納するクラス
//------------------------------------------------------------------------------
class Controller{
public:
	static const int BUTTON_MAX = 10;		// ボタンの最大数

	Controller();	// コンストラクタ
	~Controller(){ UnInit(); }	// デストラクタ
	void Init(unsigned int JoystickID = 1U, bool bKeyboard = false, bool bVibration = true, bool bLog = false, unsigned int delay = 0U, enum ReplayMode rMode = REPLAY_NONE, enum ConnectMode cMode = CONNECT_NONE);	// 初期化
	void UnInit();				// 終了処理
	void Update();				// 入力情報の更新
	int GetJoystickMax()const;	// 刺さっているジョイスティックの数の取得
	unsigned int GetJoystickID()const{ return joystickID_; }	// 関連付けされているジョイスティックの番号の取得
	void ZeroClear();			// 入力のクリア
	Vector2D GetArrow()const;	// 矢印キーの入力方向の取得(正規化済み)
	unsigned int GetUp()const{ return up_; }				// 上キー入力フレーム数の取得
	unsigned int GetDown()const{ return down_; }			// 下キー入力フレーム数の取得
	unsigned int GetLeft()const{ return left_; }			// 左キー入力フレーム数の取得
	unsigned int GetRight()const{ return right_; }			// 右キー入力フレーム数の取得
	Vector2D GetLAxis()const{ return Vector2D(lAxisX_, lAxisY_).Normalized(); }	// 左アナログスティックの入力方向の取得(正規化済み)
	float GetLAxisX()const{ return lAxisX_; }				// 左アナログスティックX軸の取得
	float GetLAxisY()const{ return lAxisY_; }				// 左アナログスティックY軸の取得
	Vector2D GetRAxis()const{ return Vector2D(rAxisX_, rAxisY_).Normalized(); }	// 右アナログスティックの入力方向の取得(正規化済み)
	float GetRAxisX()const{ return rAxisX_; }				// 右アナログスティックX軸の取得
	float GetRAxisY()const{ return rAxisY_; }				// 右アナログスティックY軸の取得
	float GetLTrigger()const{ return lTrigger_; }			// 左トリガーの取得
	float GetRTrigger()const{ return rTrigger_; }			// 右トリガーの取得
	unsigned int GetButton(unsigned int buttonID)const;		// ボタンの入力フレーム数の取得
	bool IsKeyPress(enum KeyCode keyCode)const;				// 指定したキーが押されているか
	bool SetVibration(unsigned int time, float leftSpeed = 1.0f, float rightSpeed = 1.0f);	// 振動の開始
	bool GetLogUp(unsigned int backFrame)const;				// 上キー入力のログの取得
	bool GetLogDown(unsigned int backFrame)const;			// 下キー入力のログの取得
	bool GetLogLeft(unsigned int backFrame)const;			// 左キー入力のログの取得
	bool GetLogRight(unsigned int backFrame)const;			// 右キー入力のログの取得
	float GetLogLAxisX(unsigned int backFrame)const;		// 左アナログスティックX軸のログの取得
	float GetLogLAxisY(unsigned int backFrame)const;		// 左アナログスティックY軸のログの取得
	float GetLogRAxisX(unsigned int backFrame)const;		// 右アナログスティックX軸のログの取得
	float GetLogRAxisY(unsigned int backFrame)const;		// 右アナログスティックY軸のログの取得
	float GetLogLTrigger(unsigned int backFrame)const;		// 左トリガーのログの取得
	float GetLogRTrigger(unsigned int backFrame)const;		// 右トリガーのログの取得
	bool GetLogButton(unsigned int buttonID, unsigned int backFrame)const;	// ボタン入力のログの取得
	void SetConfig(unsigned int configID, unsigned int buttonID);			// 指定したジョイスティックボタンをボタン番号に関連付ける
	void SetConfig(unsigned int configID, enum KeyCode keyCode);			// 指定したキーをボタン番号に関連付ける
	int GetConfig(unsigned int configID)const;		// ボタン番号に関連付けられたジョイスティックボタンの番号を取得する
	bool SaveReplay(const char* pFileName);			// リプレイデータの保存
	bool LoadReplay(const char* pFileName);			// リプレイデータの読み込み
	bool IsFinishReplay()const;					// リプレイが終了したかどうか

protected:
#pragma pack(push,1)
	struct InputLog{
		short keyData;	// キーの入力
		short lAxisX;	// 左アナログスティックのX軸
		short lAxisY;	// 左アナログスティックのY軸
		short rAxisX;	// 右アナログスティックのX軸
		short rAxisY;	// 右アナログスティックのY軸
		short lTrigger;	// 左トリガー
		short rTrigger;	// 右トリガー
	};
#pragma pack(pop)

	unsigned int joystickID_;				// ジョイスティック番号
	bool bKeyboard_;						// キーボードの使用フラグ

	unsigned int up_;						// 上キーの入力フレーム数
	unsigned int down_;						// 下キーの入力フレーム数
	unsigned int left_;						// 左キーの入力フレーム数
	unsigned int right_;					// 右キーの入力フレーム数
	float lAxisX_;							// 左アナログスティックのX軸
	float lAxisY_;							// 左アナログスティックのY軸
	float rAxisX_;							// 右アナログスティックのX軸
	float rAxisY_;							// 右アナログスティックのY軸
	float lTrigger_;						// 左トリガー
	float rTrigger_;						// 右トリガー
	unsigned int button_[BUTTON_MAX];		// ボタンの入力フレーム数

	bool bVibration_;						// 振動の使用フラグ
	unsigned int vibrationTime_;			// 振動時間

	bool bLog_;									// キーログの保存フラグ
	struct InputLog inputLog_[30];				// 入力履歴
	std::vector<struct InputLog> replayLog_;	// リプレイデータ
	std::vector<struct InputLog>::iterator itReplayLog_;	// リプレイデータのイテレータ
	unsigned int config_[BUTTON_MAX];			// コンフィグ(ジョイスティックボタン)
	enum KeyCode keyConfig_[BUTTON_MAX];		// コンフィグ(キーボード)
	enum ReplayMode rMode_;						// リプレイモード
	enum ConnectMode cMode_;					// 通信モード
	unsigned int delay_;						// 遅延
};

#endif	// #ifndef INCLUDE_IDEA_CONTROLLER_H