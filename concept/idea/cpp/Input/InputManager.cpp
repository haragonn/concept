/*==============================================================================
	[InputManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Input/InputManager.h"
#include "../../h/Utility/ideaUtility.h"

#include <XInput.h>
#include <stdio.h>
#include <wbemidl.h>
#include <oleauto.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "XInput9_1_0.lib")
#pragma comment(lib, "winmm.lib")

//------------------------------------------------------------------------------
// 定数定義
//------------------------------------------------------------------------------
Joystick InputManager::s_NullJoystick_;	// ジョイスティックが使われていなかった場合の空の情報

//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains 
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput 
//-----------------------------------------------------------------------------
#define SAFE_RELEASE(p) if(p){ p->Release(); p=NULL; }
BOOL IsXInputDevice(const GUID* pGuidProductFromDirectInput)
{
	IWbemLocator*           pIWbemLocator = NULL;
	IEnumWbemClassObject*   pEnumDevices = NULL;
	IWbemClassObject*       pDevices[20] = { 0 };
	IWbemServices*          pIWbemServices = NULL;
	BSTR                    bstrNamespace = NULL;
	BSTR                    bstrDeviceID = NULL;
	BSTR                    bstrClassName = NULL;
	DWORD                   uReturned = 0;
	bool                    bIsXinputDevice = false;
	UINT                    iDevice = 0;
	VARIANT                 var;
	HRESULT                 hr;

	// CoInit if needed
	hr = CoInitialize(NULL);
	bool bCleanupCOM = SUCCEEDED(hr);

	// Create WMI
	hr = CoCreateInstance(__uuidof(WbemLocator),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWbemLocator),
		(LPVOID*)&pIWbemLocator);
	if(FAILED(hr) || pIWbemLocator == NULL)
		goto LCleanup;

	bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2"); if(bstrNamespace == NULL) goto LCleanup;
	bstrClassName = SysAllocString(L"Win32_PNPEntity");   if(bstrClassName == NULL) goto LCleanup;
	bstrDeviceID = SysAllocString(L"DeviceID");          if(bstrDeviceID == NULL)  goto LCleanup;

	// Connect to WMI 
	hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L,
		0L, NULL, NULL, &pIWbemServices);
	if(FAILED(hr) || pIWbemServices == NULL)
		goto LCleanup;

	// Switch security level to IMPERSONATE. 
	CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
		RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
	if(FAILED(hr) || pEnumDevices == NULL)
		goto LCleanup;

	// Loop over all devices
	for(;;)
	{
		// Get 20 at a time
		hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
		if(FAILED(hr))
			goto LCleanup;
		if(uReturned == 0)
			break;

		for(iDevice = 0; iDevice < uReturned; iDevice++)
		{
			// For each device, get its device ID
			hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
			if(SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
			{
				// Check if the device ID contains "IG_".  If it does, then it's an XInput device
				// This information can not be found from DirectInput 
				if(wcsstr(var.bstrVal, L"IG_"))
				{
					// If it does, then get the VID/PID from var.bstrVal
					DWORD dwPid = 0, dwVid = 0;
					WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
					if(strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1)
						dwVid = 0;
					WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
					if(strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1)
						dwPid = 0;

					// Compare the VID/PID to the DInput device
					DWORD dwVidPid = MAKELONG(dwVid, dwPid);
					if(dwVidPid == pGuidProductFromDirectInput->Data1)
					{
						bIsXinputDevice = true;
						goto LCleanup;
					}
				}
			}
			SAFE_RELEASE(pDevices[iDevice]);
		}
	}

LCleanup:
	if(bstrNamespace)
		SysFreeString(bstrNamespace);
	if(bstrDeviceID)
		SysFreeString(bstrDeviceID);
	if(bstrClassName)
		SysFreeString(bstrClassName);
	for(iDevice = 0; iDevice < 20; iDevice++)
		SAFE_RELEASE(pDevices[iDevice]);
	SAFE_RELEASE(pEnumDevices);
	SAFE_RELEASE(pIWbemLocator);
	SAFE_RELEASE(pIWbemServices);

	if(bCleanupCOM)
		CoUninitialize();

	return bIsXinputDevice;
}

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
InputManager::InputManager() :
	hInst_(0),
	hWnd_(0),
	pDInput_(nullptr),
	pKeyboardDev_(nullptr),
	pMouseDev_(nullptr),
	joystickNum_(0),
	joystickDevNum_(0),
	maxJoystickNum_(0),
	pJoystick_(nullptr)
{
	ZeroMemory(keyState_, sizeof(keyState_));
	ZeroMemory(&mouseState_, sizeof(mouseState_));

	// ジョイスティック
	for(int i = JOYSTIC_MAX - 1; i > 0; --i){ pJoystickDev_[i] = nullptr; }
}

//------------------------------------------------------------------------------
// 初期化
// 引数　：インスタンス(HINSTANCE hInst),ウィンドウハンドル(HWND hWnd)
// 戻り値：成否
//------------------------------------------------------------------------------
bool InputManager::Init(HINSTANCE hInst, HWND hWnd)
{
	// 引数を反映
	hInst_ = hInst;
	hWnd_ = hWnd;

	// DirectInput8オブジェクトの生成
	HRESULT hr;
	hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&pDInput_, nullptr);
	if(FAILED(hr)){ return false; }

	//========================================
	// キーボード
	//========================================
	// デバイスの生成
	hr = pDInput_->CreateDevice(GUID_SysKeyboard, &pKeyboardDev_, nullptr);
	if(FAILED(hr)){ return false; }

	// データフォーマットの設定
	hr = pKeyboardDev_->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr)){ return false; }

	// 協調レベルの設定（非排他モード,フォアグラウンド,Windowsキーの無効）
	hr = pKeyboardDev_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND | DISCL_NOWINKEY);
	if(FAILED(hr)){ return false; }

	// アクセス権の取得
	pKeyboardDev_->Acquire();

	//========================================
	// マウス
	//========================================
	// デバイスの生成
	hr = pDInput_->CreateDevice(GUID_SysMouse, &pMouseDev_, nullptr);
	if(FAILED(hr)){ return false; }

	// データフォーマットの設定
	hr = pMouseDev_->SetDataFormat(&c_dfDIMouse);
	if(FAILED(hr)){ return false; }

	// 協調レベルの設定（非排他モード,フォアグラウンド）
	hr = pMouseDev_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if(FAILED(hr)){ return false; }

	// 入力デバイス属性の設定
	DIPROPDWORD dimouse;
	ZeroMemory(&dimouse, sizeof(dimouse));
	dimouse.diph.dwSize = sizeof(dimouse);
	dimouse.diph.dwHeaderSize = sizeof(dimouse.diph);
	dimouse.diph.dwObj = 0;
	dimouse.diph.dwHow = DIPH_DEVICE;
	dimouse.dwData = DIPROPAXISMODE_REL;
	hr = pMouseDev_->SetProperty(DIPROP_AXISMODE, &dimouse.diph);
	if(FAILED(hr)){ return false; }

	// アクセス権の取得
	pMouseDev_->Acquire();

	//========================================
	// ジョイスティック
	//========================================
	// 刺さっているジョイスティックの数を取得
	maxJoystickNum_ = min(JOYSTIC_MAX, joyGetNumDevs());

	// コントローラデバイス列挙コールバック関数の設定
	hr = pDInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
	if(FAILED(hr)){ return false; }

	// 入力デバイス属性の設定
	DIPROPDWORD diprop;
	ZeroMemory(&diprop, sizeof(diprop));
	diprop.diph.dwSize = sizeof(DIPROPDWORD);
	diprop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_ABS;

	for(int i = (int)joystickDevNum_ - 1; i >= 0; --i){
		// データフォーマットの設定
		hr = pJoystickDev_[i]->SetDataFormat(&c_dfDIJoystick);
		if(FAILED(hr)){ return false; }

		// 協調レベルの設定（非排他モード,バックグラウンド）
		hr = pJoystickDev_[i]->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		if(FAILED(hr)){ return false; }

		// 軸モードの設定
		hr = pJoystickDev_[i]->EnumObjects(EnumAxesCallback, pJoystickDev_[i], DIDFT_AXIS);
		if(FAILED(hr)){ return false; }

		// 入力デバイス属性の適用
		hr = pJoystickDev_[i]->SetProperty(DIPROP_AXISMODE, &diprop.diph);
		if(FAILED(hr)){ return false; }

		// アクセス権の取得
		pJoystickDev_[i]->Acquire();
	}

	// ジョイスティックインスタンス用意
	pJoystick_ = new Joystick[JOYSTIC_MAX];

	// ジョイスティックインスタンス初期化
	for(int i = 4 - 1; i >= 0; --i){
		ZeroMemory(&pJoystick_[i], sizeof(Joystick));
	}

	return true;
}

//------------------------------------------------------------------------------
// 終了処理
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void InputManager::UnInit()
{
	// ジョイスティック
	for(DWORD i = 0; i < joystickDevNum_; ++i){
		if(pJoystickDev_[i]){
			pJoystickDev_[i]->Unacquire();
			SafeRelease(pJoystickDev_[i]);
		}
	}
	SafeDeleteArray(pJoystick_);

	// マウス
	if(pMouseDev_){
		pMouseDev_->Unacquire();
		SafeRelease(pMouseDev_);
	}

	// キーボード
	if(pKeyboardDev_){
		pKeyboardDev_->Unacquire();
		SafeRelease(pKeyboardDev_);
	}

	// DirectInput
	SafeRelease(pDInput_);
}

//------------------------------------------------------------------------------
// 全ての入力情報の更新
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool InputManager::UpdateAll()
{
	bool bKey = UpdateKeyState();
	bool bMouse = UpdateMouseState();
	bool bJoystick = UpdateJoystick();

	return (bKey && bMouse && bJoystick);
}

//------------------------------------------------------------------------------
// キーボードの入力情報の更新
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool InputManager::UpdateKeyState()
{
	if(!pKeyboardDev_){ return true; }	// キーボードがつかわれていなければ終了

	ZeroMemory(keyState_, sizeof(keyState_));	// ゼロクリア
	if(FAILED(pKeyboardDev_->GetDeviceState(sizeof(keyState_), keyState_))){	// 入力情報更新
		if(FAILED(pKeyboardDev_->Acquire()) || FAILED(pKeyboardDev_->GetDeviceState(sizeof(keyState_), keyState_))){ return false; }	// 再度アクセス権を取得してから更新
	}

	return true;
}

//------------------------------------------------------------------------------
// マウスの入力情報の更新
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool InputManager::UpdateMouseState()
{
	if(!pMouseDev_){ return true; }	// マウスがつかわれていなければ終了

	// APIからマウスの位置を取得
	POINT tPoint;
	GetCursorPos(&tPoint);
	ScreenToClient(hWnd_, &tPoint);
	mouseState_.posX = tPoint.x;
	mouseState_.posY = tPoint.y;

	DIMOUSESTATE ms;
	if(FAILED(pMouseDev_->GetDeviceState(sizeof(DIMOUSESTATE), &ms))){	// 入力情報更新
		if(FAILED(pMouseDev_->Acquire()) || FAILED(pMouseDev_->GetDeviceState(sizeof(DIMOUSESTATE), &ms))){ return false; }	// 再度アクセス権を取得してから更新
	}
	// ボタンが押されていたらフレーム数を加算
	for(int i = Mouse::BUTTON_MAX - 1; i >= 0; --i){
		++mouseState_.button[i];
		if(!(ms.rgbButtons[i] & 0x80)){ mouseState_.button[i] = 0; }
	}
	mouseState_.wheel = ms.lZ;

	return true;
}

//------------------------------------------------------------------------------
// ジョイスティックの入力情報の更新
// 引数　：なし
// 戻り値：成否
//------------------------------------------------------------------------------
bool InputManager::UpdateJoystick()
{
	joystickNum_ = 0;

	//========================================
	// DirectInput
	//========================================
	for(DWORD i = 0; i < joystickDevNum_; ++i)
	{
		if(!pJoystickDev_[i]){ continue; }

		if(FAILED(pJoystickDev_[i]->Poll())){
			if(FAILED(pJoystickDev_[i]->Acquire()) || FAILED(pJoystickDev_[i]->Poll())){ continue; }
		}

		DIJOYSTATE js;
		if(FAILED(pJoystickDev_[i]->GetDeviceState(sizeof(DIJOYSTATE), &js))){ continue; }

		// 入力情報格納
		if((js.lX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			js.lX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)){
			js.lX = 0;
		}
		if((js.lY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			js.lY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)){
			js.lY = 0;
		}

		if((js.lZ < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			js.lZ > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)){
			js.lZ = 0;
		}
		if((js.lRz < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			js.lRz > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)){
			js.lRz = 0;
		}

		pJoystick_[joystickNum_].lAxisX = js.lX > 0 ? (float)(js.lX) / 32767.0f : (float)(js.lX) / 32768.0f;
		pJoystick_[joystickNum_].lAxisY = js.lY > 0 ? (float)(js.lY) / 32767.0f : (float)(js.lY) / 32768.0f;
		pJoystick_[joystickNum_].rAxisX = js.lZ > 0 ? (float)(js.lZ) / 32767.0f : (float)(js.lZ) / 32768.0f;
		pJoystick_[joystickNum_].rAxisY = js.lRz > 0 ? (float)(js.lRz) / 32767.0f : (float)(js.lRz) / 32768.0f;

		pJoystick_[joystickNum_].up = (js.lY < 0);
		pJoystick_[joystickNum_].down = (js.lY > 0);
		pJoystick_[joystickNum_].right = (js.lX > 0);
		pJoystick_[joystickNum_].left = (js.lX < 0);
		for(int j = Joystick::BUTTON_MAX - 1; j >= 0; --j){
			pJoystick_[joystickNum_].button[j] = !!(js.rgbButtons[j] & 0x80);
		}
		++joystickNum_;
	}

	//========================================
	// XInput
	//========================================
	XINPUT_STATE state;
	DWORD Temp = joystickNum_;
	for(int i = (int)joystickNum_; i < JOYSTIC_MAX; ++i){
		if(XInputGetState(i - Temp, &state) == ERROR_SUCCESS){
			// 入力情報格納
			if((state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
				state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)){
				state.Gamepad.sThumbLX = 0;
			}
			if((state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
				state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)){
				state.Gamepad.sThumbLY = 0;
			}
			if((state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
				state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)){
				state.Gamepad.sThumbRX = 0;
			}
			if((state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
				state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)){
				state.Gamepad.sThumbRY = 0;
			}

			pJoystick_[joystickNum_].lAxisX = state.Gamepad.sThumbLX > 0 ? (float)(state.Gamepad.sThumbLX) / 32767.0f : (float)(state.Gamepad.sThumbLX) / 32768.0f;
			pJoystick_[joystickNum_].lAxisY = state.Gamepad.sThumbLY > 0 ? (float)(state.Gamepad.sThumbLY) / 32767.0f : (float)(state.Gamepad.sThumbLY) / 32768.0f;
			pJoystick_[joystickNum_].rAxisX = state.Gamepad.sThumbRX > 0 ? (float)(state.Gamepad.sThumbRX) / 32767.0f : (float)(state.Gamepad.sThumbRX) / 32768.0f;
			pJoystick_[joystickNum_].rAxisY = state.Gamepad.sThumbRY > 0 ? (float)(state.Gamepad.sThumbRY) / 32767.0f : (float)(state.Gamepad.sThumbRY) / 32768.0f;

			pJoystick_[joystickNum_].lTrigger = (float)(state.Gamepad.bLeftTrigger) / 255.0f;
			pJoystick_[joystickNum_].rTrigger = (float)(state.Gamepad.bRightTrigger) / 255.0f;

			pJoystick_[joystickNum_].up = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP);
			pJoystick_[joystickNum_].down = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
			pJoystick_[joystickNum_].left = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
			pJoystick_[joystickNum_].right = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);

			pJoystick_[joystickNum_].button[0] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_A);
			pJoystick_[joystickNum_].button[1] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_B);
			pJoystick_[joystickNum_].button[2] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_X);
			pJoystick_[joystickNum_].button[3] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_Y);
			pJoystick_[joystickNum_].button[4] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
			pJoystick_[joystickNum_].button[5] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
			pJoystick_[joystickNum_].button[6] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_START);
			pJoystick_[joystickNum_].button[7] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK);
			pJoystick_[joystickNum_].button[8] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB);
			pJoystick_[joystickNum_].button[9] = !!(state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB);
			++joystickNum_;
		}
	}
	return true;
}

//------------------------------------------------------------------------------
// ジョイスティックを振動させる
// 引数　：ジョイスティックのID(DWORD JoystickID)
// 戻り値：成否
//------------------------------------------------------------------------------
bool InputManager::SetVibration(DWORD joystickID, float leftSpeed, float rightSpeed)
{
	if(joystickID - 1 > (DWORD)GetJoystickNum()){ return false; }

	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = (WORD)(65535 * min(1.0f, max(0.0f, leftSpeed)));
	vibration.wRightMotorSpeed = (WORD)(65535 * min(1.0f, max(0.0f, rightSpeed)));
	return SUCCEEDED(XInputSetState(joystickID - 1, &vibration));
}

//------------------------------------------------------------------------------
// ジョイスティックの入力情報の取得
// 引数　：ジョイスティックのID(DWORD JoystickID)
// 戻り値：ジョイスティックの構造体
//------------------------------------------------------------------------------
Joystick& InputManager::GetJoystick(DWORD JoystickID)
{
	if(JoystickID >= JOYSTIC_MAX){ return s_NullJoystick_; }	// ないものを要求されたら空の構造体を返す
	return pJoystick_[JoystickID];	// 構造体を返す
}

//------------------------------------------------------------------------------
// ジョイスティック列挙のコールバック関数
// 引数　：インスタンス(LPCDIDEVICEINSTANCE pInst),パラメーター(LPVOID pvRef)
// 戻り値：続行するかどうか
//------------------------------------------------------------------------------
BOOL CALLBACK InputManager::EnumJoysticksCallback(LPCDIDEVICEINSTANCE pInst, LPVOID pvRef)
{
	if(IsXInputDevice(&pInst->guidProduct)){ return DIENUM_CONTINUE; }	// XInput対応ならばスルー

	InputManager* im = (InputManager*)pvRef;
	im->devCaps_.dwSize = sizeof(DIDEVCAPS);
	im->pDInput_->CreateDevice(pInst->guidInstance, &im->pJoystickDev_[im->joystickDevNum_], NULL);
	im->pJoystickDev_[im->joystickDevNum_]->GetCapabilities(&im->devCaps_);

	// ジョイスティック数が最大になるまで列挙する
	if(++(im->joystickDevNum_) == im->maxJoystickNum_){ return DIENUM_STOP; }
	return DIENUM_CONTINUE;
}

//------------------------------------------------------------------------------
// アナログスティックのコールバック関数
// 引数　：インスタンス(LPCDIDEVICEINSTANCE pInst),パラメーター(LPVOID pvRef)
// 戻り値：続行するかどうか
//------------------------------------------------------------------------------
BOOL CALLBACK InputManager::EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE pInst, LPVOID pvRef)
{
	DIPROPRANGE diprg;
	ZeroMemory(&diprg, sizeof(DIPROPRANGE));
	diprg.diph.dwSize = sizeof(DIPROPRANGE);
	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diprg.diph.dwObj = pInst->dwType;
	diprg.diph.dwHow = DIPH_BYID;
	diprg.lMin = -32768;
	diprg.lMax = 32767;

	LPDIRECTINPUTDEVICE8 pJoyStickDev = (LPDIRECTINPUTDEVICE8)pvRef;
	if(FAILED(pJoyStickDev->SetProperty(DIPROP_RANGE, &diprg.diph))){ return DIENUM_STOP; }
	return DIENUM_CONTINUE;
}