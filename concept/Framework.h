/*==============================================================================
	[Framework.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FLAMEWORK_H
#define INCLUDE_IDEA_FLAMEWORK_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "idea/h/Framework/BaseWindow.h"
#include <atomic>

//------------------------------------------------------------------------------
// �O���錾
//------------------------------------------------------------------------------
class Scene;

//------------------------------------------------------------------------------
// �N���X���@�FFramework
// �N���X�T�v�F�V���O���g��
// �@�@�@�@�@�@�E�B���h�E�̊Ǘ�,�Q�[���̃��C�����[�v���s��
//------------------------------------------------------------------------------
class Framework : public BaseWindow{
public:
	static Framework& Instance()	// �B��̃C���X�^���X��Ԃ�
	{
		static Framework s_Instance;
		return s_Instance;
	}
	bool Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed = true, UINT fps = 60U, UINT frameSkipMax = 0U, bool bMSAA = false, bool bNetwork = false, const char* pClassName = "idea");	// ������
	void UnInit();				// �I������
	void Run(Scene* pInitScene);	// �V�[���̎��s
	float GetFps(){ return fps_; }				// FPS�̎擾
	UINT GetTargetFps(){ return targetFps_; }	// �ڕWFPS�̎擾
	void SetFpsSimpleDisplayFlag(bool bShow){ bFpsShow_ = bShow; }	// FPS�ȈՕ\���̐ݒ�
	void SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen);	// �}�E�X�J�[�\���̉����̐ݒ�

private:
	HINSTANCE hInst_;			// �C���X�^���X�n���h��

	UINT width_;				// �X�N���[���̕�
	UINT height_;				// �X�N���[���̍���
	bool bWindowed_;			// �E�B���h�E���[�h

	std::atomic<bool> bReady_;			// ���s����
	std::atomic<Scene*> pInitScene_;	// �����V�[��

	bool bOnWindowed_;			// �E�B���h�E���̃}�E�X�J�[�\��
	bool bOnFullscreen_;		// �t���X�N���[�����̃}�E�X�J�[�\��

	std::atomic<bool> bChangeDispRequest_;	// �E�B���h�E���[�h�̐؂�ւ��v��
	std::atomic<bool> bChangeDispReady_;	// �E�C���h�E���[�h�̐؂�ւ��̊���

	std::atomic<UINT> targetFps_;		// �ڕW��FPS
	float fps_;							// ���݂�FPS
	bool bFpsShow_;						// FPS�̊ȈՕ\���t���O
	std::atomic<UINT> frameSkipMax_;	// �ő�t���[���X�L�b�v��
	std::atomic<UINT> frameSkipCount_;	// �t���[���X�L�b�v��

	bool bStorage_;				// �X�g���[�W�̎g�p�\�t���O

	HANDLE hMainLoopThread_;	// ���C�����[�v�X���b�h�̃n���h��

	friend DWORD WINAPI MainLoopThread(void* vp);	// ���C�����[�v�X���b�h
	void MainLoop();			// ���C�����[�v
	void ChangeDisplayMode();	// �E�B���h�E���[�h�̐؂�ւ�

	// ���b�Z�[�W����
	LRESULT OnDestroy();	// �E�B���h�E���j������鎞
	LRESULT OnClose();		// �E�B���h�E������ꂽ��
	LRESULT OnKeyDown();	// �L�[�����͂��ꂽ��
	LRESULT OnSysKeyDown();	// �V�X�e���L�[�����͂��ꂽ��
	LRESULT OnSetCursor();	// �E�B���h�E���Ń}�E�X�J�[�\������������

	Framework();			// �R���X�g���N�^
	~Framework(){}			// �f�X�g���N�^

	// �R�s�[�R���X�g���N�^�̋֎~
	Framework(Framework& src){}
	Framework& operator=(Framework& src){}
};

#endif // #ifndef INCLUDE_IDEA_FLAMEWORK_H