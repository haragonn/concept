#include "../../h/Archive/PathManager.h"
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

LRESULT PathManager::PushPath(const char* pszPath)
{
	std::string path;
	if (::PathIsRelative(pszPath))
	{
		//	相対パスの場合、カレントパスと連結する.
		path = GetPath();
		path += pszPath;
	}
	else
	{
		//	絶対パスの場合は、カレントパスは無視する.
		path = pszPath;
	}

	//	パスの正規化を行う.
	if (0 != NormalizePath(path))
	{
		return 1;
	}
	//	末尾には'\\'が必ずあるようにしておく.
	if (!path.empty() && '\\' != path[path.size() - 1])
	{
		path += '\\';
	}

	//	スタックにパスを登録.
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
