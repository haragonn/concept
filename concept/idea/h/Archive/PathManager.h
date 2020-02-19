#pragma once

#include "IPathManager.h"
#include <vector>
#include <string>

class PathManager : public IPathManager
{
	std::vector<std::string> paths_;
public:
	/**
	�X�^�b�N�Ƀp�X��o�^����.
	��΃p�X��o�^�����ꍇ�́A�J�����g�p�X�͐�΃p�X�ƂȂ�.
	���΃p�X��o�^�����ꍇ�́A�J�����g�p�X�͓o�^�O�̃J�����g�p�X�ɑ��΃p�X��K�p�����p�X�ƂȂ�.
	@param	path	�o�^����p�X.
	@return			�G���[�R�[�h.
	*/
	virtual LRESULT PushPath(const char* path);

	/**
	�X�^�b�N����ŐV�̓o�^�p�X����菜��.
	�Ō�ɐ�������pushPath(const WCHAR *path)�̉e������菜��.
	@return			�G���[�R�[�h. �X�^�b�N��1���p�X�������ꍇ�ɌĂяo���ꂽ�ꍇ�A�G���[�ƂȂ�.
	*/
	virtual LRESULT PopPath();

	/**
	���݂̃J�����g�p�X���擾����.
	@return			���݂̃J�����g�p�X������ւ̃|�C���^.
	*/
	virtual const char* GetPath()const;
};
