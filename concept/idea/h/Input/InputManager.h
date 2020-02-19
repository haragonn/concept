/*==============================================================================
	[InputManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_INPUTMANAGER_H
#define INCLUDE_IDEA_INPUTMANAGER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

//------------------------------------------------------------------------------
// 構造体定義
//------------------------------------------------------------------------------
#pragma pack(push,1)

struct Mouse{
	static const int BUTTON_MAX = 4;	// ボタンの最大数
	int posX;	// X座標
	int posY;	// Y座標
	unsigned int button[BUTTON_MAX];	// ボタンの押されたフレーム数
	int wheel;	// ホイールの差分
};

struct Joystick{
	static const int BUTTON_MAX = 10;	// ボタンの最大数
	bool button[BUTTON_MAX];	// ボタンが押されたかどうか
	bool up;		// 上キーが押されたかどうか
	bool down;		// 下キーが押されたかどうか
	bool left;		// 左キーが押されたかどうか
	bool right;		// 右キーが押されたかどうか
	float lAxisX;		// 左アナログスティックのX軸
	float lAxisY;		// 左アナログスティックのY軸
	float rAxisX;		// 右アナログスティックのX軸
	float rAxisY;		// 右アナログスティックのY軸
	float lTrigger;	// 左トリガー
	float rTrigger;	// 右トリガー
};

#pragma pack(pop)

//------------------------------------------------------------------------------
// クラス名　：InputManager
// クラス概要：シングルトン
// 　　　　　　DirectInputデバイスの管理を行う
// 　　　　　　キーボード,マウス,ジョイスティックの入力情報を更新,管理する
//------------------------------------------------------------------------------
class InputManager{
public:
	static const int JOYSTIC_MAX = 4;	// ジョイスティックの最大数

	static InputManager& Instance()	// 唯一のインスタンスを返す
	{
		static InputManager s_Instance;
		return s_Instance;
	}

	//========================================
	// DirectInput
	//========================================
	bool Init(HINSTANCE hInst, HWND hWnd);	// 初期化
	void UnInit();						// 終了処理
	bool UpdateAll();					// 全ての入力情報の更新

	//========================================
	// キーボード
	//========================================
	inline bool UpdateKeyState();				// キーボードの入力情報の更新
	bool IsKeyPress(const DWORD keyCode){ return !!(keyState_[keyCode] & 0x80); }	// 指定したキーが押されているか

	//========================================
	// マウス
	//========================================
	inline bool UpdateMouseState();				// マウスの入力情報の更新
	Mouse& GetMouse(){ return mouseState_; }	// マウスの入力情報の取得

	//========================================
	// ジョイスティック
	//========================================
	inline bool UpdateJoystick();					// ジョイスティックの入力情報の更新
	int GetJoystickNum(){ return joystickNum_; }	// 刺さっているジョイスティックの数の取得
	bool SetVibration(DWORD joystickID, float leftSpeed, float rightSpeed);	// ジョイスティックを振動させる
	Joystick& GetJoystick(DWORD JoystickID);		// ジョイスティックの入力情報の取得

private:
	// ジョイスティックのコールバック関数
	static BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE pInst, LPVOID pvRef);	// ジョイスティック列挙のコールバック関数
	static BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE pInst, LPVOID pvRef);	// アナログスティックのコールバック関数

	HINSTANCE hInst_;			// インスタンス
	HWND hWnd_;					// ウィンドウハンドル

	// DirectInput
	LPDIRECTINPUT8 pDInput_;	// インターフェイス
	DIDEVCAPS devCaps_;			// デバイスの能力

	// キーボード
	LPDIRECTINPUTDEVICE8 pKeyboardDev_;	// キーボードのデバイス
	BYTE keyState_[256];				// キーボードの入力情報

	// マウス
	LPDIRECTINPUTDEVICE8 pMouseDev_;	// マウスのデバイス
	Mouse mouseState_;					// マウスの入力情報

	// ジョイスティック
	static Joystick s_NullJoystick_;					// ジョイスティックが使われていなかった場合の空の情報
	LPDIRECTINPUTDEVICE8 pJoystickDev_[JOYSTIC_MAX];	// ジョイスティックのデバイス
	DWORD joystickNum_;			// 刺さってるジョイスティックの数
	DWORD joystickDevNum_;		// ジョイスティックデバイスの数
	DWORD maxJoystickNum_;		// ジョイスティックの最大数
	Joystick* pJoystick_;		// ジョイスティックの入力情報

	InputManager();					// コンストラクタ
	~InputManager(){ UnInit(); }	// デストラクタ

	// コピーコンストラクタの禁止
	InputManager(const InputManager& src){}
	InputManager& operator=(const InputManager& src){}
};

#endif // #ifndef INCLUDE_IDEA_INPUTMANAGER_H