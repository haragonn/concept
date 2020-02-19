#include "../../h/Archive/PathManager.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

LRESULT PathManager::PushPath(const char* pszPath)
{
	std::string path;
	if (::PathIsRelative(pszPath))
	{
		//	���΃p�X�̏ꍇ�A�J�����g�p�X�ƘA������.
		path = GetPath();
		path += pszPath;
	}
	else
	{
		//	��΃p�X�̏ꍇ�́A�J�����g�p�X�͖�������.
		path = pszPath;
	}

	//	�p�X�̐��K�����s��.
	if (0 != NormalizePath(path))
	{
		return 1;
	}
	//	�����ɂ�'\\'���K������悤�ɂ��Ă���.
	if (!path.empty() && '\\' != path[path.size() - 1])
	{
		path += '\\';
	}

	//	�X�^�b�N�Ƀp�X��o�^.
	paths_.push_back(path);
	return 0;
}

LRESULT PathManager::PopPath()
{
	if (paths_.empty())
	{
		return 1;
	}
	paths_.pop_back();
	return 0;
}

const char* PathManager::GetPath()const
{
	const static char tmp[] = "";
	if (paths_.empty())
		return tmp;
	return paths_.back().c_str();
}
