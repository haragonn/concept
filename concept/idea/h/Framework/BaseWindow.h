/*==============================================================================
	[BaseWindow.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_BASEWINDOW_H
#define INCLUDE_IDEA_BASEWINDOW_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//------------------------------------------------------------------------------
// �N���X���@�FBaseWindow
// �N���X�T�v�F�E�B���h�E������Window�A�v���P�[�V�����̊��ƂȂ�N���X
// �@�@�@�@�@�@��Ƀ��b�Z�[�W�Ǘ����s��
//------------------------------------------------------------------------------
class BaseWindow{
public:
	BaseWindow();			// �R���X�g���N�^
	virtual ~BaseWindow(){}	// �f�X�g���N�^
	bool SetWindowFormatText(const char* pFormat, ...);	// �E�B���h�E�^�C�g���̕ύX

protected:
	HWND hWnd_;			// �E�B���h�E�n���h��
	WNDCLASSEX wcex_;	// �E�B���h�E�N���X
	UINT msg_;			// ���b�Z�[�W
	WPARAM wp_;			// �p�����[�^
	LPARAM lp_;			// �p�����[�^
	char title_[256];	// �E�C���h�E�^�C�g��

	bool ProcessMessage();	// ���b�Z�[�W�̊Ď�

	// ���b�Z�[�W�v���V�[�W���֘A
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);	// �E�B���h�E�v���V�[�W��
	virtual inline LRESULT MessageProc();										// ���b�Z�[�W�̊Ǘ�
	virtual LRESULT OnCreate()		{ return DefaultProc(); }					// �E�B���h�E���������ꂽ��
	virtual LRESULT OnClose()		{ return DefaultProc(); }					// �E�B���h�E������ꂽ��
	virtual LRESULT OnDestroy()		{ PostQuitMessage(0); return 0; }			// �E�B���h�E���j������鎞
	virtual LRESULT OnActivate()	{ return DefaultProc(); }					// �E�B���h�E�̃A�N�e�B�u��,��A�N�e�B�u���̒ʒm
	virtual LRESULT OnSetFocus()	{ return DefaultProc(); }					// �L�[�{�[�h�t�H�[�J�X�𓾂���
	virtual LRESULT OnKillFocus()	{ return DefaultProc(); }					// �L�[�{�[�h�t�H�[�J�X����������
	virtual LRESULT OnMove()		{ return DefaultProc(); }					// �E�B���h�E���ړ����ꂽ��
	virtual LRESULT OnSize()		{ return DefaultProc(); }					// �E�B���h�E�̃T�C�Y���ύX���ꂽ��
	virtual LRESULT OnPaint()		{ return DefaultProc(); }					// �`�悷�鎞
	virtual LRESULT OnKeyDown()		{ return DefaultProc(); }					// �L�[�����͂��ꂽ��
	virtual LRESULT OnSysKeyDown()	{ return DefaultProc(); }					// �V�X�e���L�[�����͂��ꂽ��
	virtual LRESULT OnSetCursor()	{ return DefaultProc(); }					// �E�B���h�E���Ń}�E�X�J�[�\������������
	virtual LRESULT DefaultProc()	{ return DefWindowProc(hWnd_, msg_, wp_, lp_); }	// �f�t�H���g�̏���
};

#endif // #ifndef INCLUDE_IDEA_BASEWINDOW_H