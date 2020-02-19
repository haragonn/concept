#pragma once
#include <windows.h>
#include <string>
#include <memory>

class IPathManager
{
public:
	/**
	�f�X�g���N�^.
	*/
	virtual ~IPathManager(){};

	/**
	�X�^�b�N�Ƀp�X��o�^����.
	��΃p�X��o�^�����ꍇ�́A�J�����g�p�X�͐�΃p�X�ƂȂ�.
	���΃p�X��o�^�����ꍇ�́A�J�����g�p�X�͓o�^�O�̃J�����g�p�X�ɑ��΃p�X��K�p�����p�X�ƂȂ�.
	@param	path	�o�^����p�X.
	@return			�G���[�R�[�h.
	*/
	virtual LRESULT PushPath(const char* path) = 0;

	/**
	�X�^�b�N����ŐV�̓o�^�p�X����菜��.
	�Ō�ɐ�������pushPath(const WCHAR *path)�̉e������菜��.
	@return			�G���[�R�[�h. �X�^�b�N��1���p�X�������ꍇ�ɌĂяo���ꂽ�ꍇ�A�G���[�ƂȂ�.
	*/
	virtual LRESULT PopPath() = 0;

	/**
	���݂̃J�����g�p�X���擾����.
	@return			���݂̃J�����g�p�X������ւ̃|�C���^.
	*/
	virtual const char* GetPath()const = 0;

	/**
	�p�X�𐳋K������.
	'/'��'\\'�ɕϊ�������ŁA�s�v��".", ".."���܂܂Ȃ��p�X�ɕϊ������.
	�G���[�̏ꍇ��path�ɕω��͂����Ȃ�.
	@param	path	���K������p�X������. ���\�b�h�����������ꍇ�̂݁A���K�����ꂽ�p�X������ɏ����ς��.
	@return			�G���[�R�[�h.
	*/
	static LRESULT NormalizePath(std::string& path);
};

typedef std::shared_ptr<IPathManager> SPathManager;