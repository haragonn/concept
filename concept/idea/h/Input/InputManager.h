/*==============================================================================
	[InputManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_INPUTMANAGER_H
#define INCLUDE_IDEA_INPUTMANAGER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

//------------------------------------------------------------------------------
// �\���̒�`
//------------------------------------------------------------------------------
#pragma pack(push,1)

struct Mouse{
	static const int BUTTON_MAX = 4;	// �{�^���̍ő吔
	int posX;	// X���W
	int posY;	// Y���W
	unsigned int button[BUTTON_MAX];	// �{�^���̉����ꂽ�t���[����
	int wheel;	// �z�C�[���̍���
};

struct Joystick{
	static const int BUTTON_MAX = 10;	// �{�^���̍ő吔
	bool button[BUTTON_MAX];	// �{�^���������ꂽ���ǂ���
	bool up;		// ��L�[�������ꂽ���ǂ���
	bool down;		// ���L�[�������ꂽ���ǂ���
	bool left;		// ���L�[�������ꂽ���ǂ���
	bool right;		// �E�L�[�������ꂽ���ǂ���
	float lAxisX;		// ���A�i���O�X�e�B�b�N��X��
	float lAxisY;		// ���A�i���O�X�e�B�b�N��Y��
	float rAxisX;		// �E�A�i���O�X�e�B�b�N��X��
	float rAxisY;		// �E�A�i���O�X�e�B�b�N��Y��
	float lTrigger;	// ���g���K�[
	float rTrigger;	// �E�g���K�[
};

#pragma pack(pop)

//------------------------------------------------------------------------------
// �N���X���@�FInputManager
// �N���X�T�v�F�V���O���g��
// �@�@�@�@�@�@DirectInput�f�o�C�X�̊Ǘ����s��
// �@�@�@�@�@�@�L�[�{�[�h,�}�E�X,�W���C�X�e�B�b�N�̓��͏����X�V,�Ǘ�����
//------------------------------------------------------------------------------
class InputManager{
public:
	static const int JOYSTIC_MAX = 4;	// �W���C�X�e�B�b�N�̍ő吔

	static InputManager& Instance()	// �B��̃C���X�^���X��Ԃ�
	{
		static InputManager s_Instance;
		return s_Instance;
	}

	//========================================
	// DirectInput
	//========================================
	bool Init(HINSTANCE hInst, HWND hWnd);	// ������
	void UnInit();						// �I������
	bool UpdateAll();					// �S�Ă̓��͏��̍X�V

	//========================================
	// �L�[�{�[�h
	//========================================
	inline bool UpdateKeyState();				// �L�[�{�[�h�̓��͏��̍X�V
	bool IsKeyPress(const DWORD keyCode){ return !!(keyState_[keyCode] & 0x80); }	// �w�肵���L�[��������Ă��邩

	//========================================
	// �}�E�X
	//========================================
	inline bool UpdateMouseState();				// �}�E�X�̓��͏��̍X�V
	Mouse& GetMouse(){ return mouseState_; }	// �}�E�X�̓��͏��̎擾

	//========================================
	// �W���C�X�e�B�b�N
	//========================================
	inline bool UpdateJoystick();					// �W���C�X�e�B�b�N�̓��͏��̍X�V
	int GetJoystickNum(){ return joystickNum_; }	// �h�����Ă���W���C�X�e�B�b�N�̐��̎擾
	bool SetVibration(DWORD joystickID, float leftSpeed, float rightSpeed);	// �W���C�X�e�B�b�N��U��������
	Joystick& GetJoystick(DWORD JoystickID);		// �W���C�X�e�B�b�N�̓��͏��̎擾

private:
	// �W���C�X�e�B�b�N�̃R�[���o�b�N�֐�
	static BOOL CALLBACK EnumJoysticksCallback(LPCDIDEVICEINSTANCE pInst, LPVOID pvRef);	// �W���C�X�e�B�b�N�񋓂̃R�[���o�b�N�֐�
	static BOOL CALLBACK EnumAxesCallback(LPCDIDEVICEOBJECTINSTANCE pInst, LPVOID pvRef);	// �A�i���O�X�e�B�b�N�̃R�[���o�b�N�֐�

	HINSTANCE hInst_;			// �C���X�^���X
	HWND hWnd_;					// �E�B���h�E�n���h��

	// DirectInput
	LPDIRECTINPUT8 pDInput_;	// �C���^�[�t�F�C�X
	DIDEVCAPS devCaps_;			// �f�o�C�X�̔\��

	// �L�[�{�[�h
	LPDIRECTINPUTDEVICE8 pKeyboardDev_;	// �L�[�{�[�h�̃f�o�C�X
	BYTE keyState_[256];				// �L�[�{�[�h�̓��͏��

	// �}�E�X
	LPDIRECTINPUTDEVICE8 pMouseDev_;	// �}�E�X�̃f�o�C�X
	Mouse mouseState_;					// �}�E�X�̓��͏��

	// �W���C�X�e�B�b�N
	static Joystick s_NullJoystick_;					// �W���C�X�e�B�b�N���g���Ă��Ȃ������ꍇ�̋�̏��
	LPDIRECTINPUTDEVICE8 pJoystickDev_[JOYSTIC_MAX];	// �W���C�X�e�B�b�N�̃f�o�C�X
	DWORD joystickNum_;			// �h�����Ă�W���C�X�e�B�b�N�̐�
	DWORD joystickDevNum_;		// �W���C�X�e�B�b�N�f�o�C�X�̐�
	DWORD maxJoystickNum_;		// �W���C�X�e�B�b�N�̍ő吔
	Joystick* pJoystick_;		// �W���C�X�e�B�b�N�̓��͏��

	InputManager();					// �R���X�g���N�^
	~InputManager(){ UnInit(); }	// �f�X�g���N�^

	// �R�s�[�R���X�g���N�^�̋֎~
	InputManager(const InputManager& src){}
	InputManager& operator=(const InputManager& src){}
};

#endif // #ifndef INCLUDE_IDEA_INPUTMANAGER_H