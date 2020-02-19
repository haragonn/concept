#include "../../h/Archive/VirtualFileLoader.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

using namespace std;

VirtualFileLoader::LoaderPair::LoaderPair(SFileLoader _loader, std::string _path)
:path(_path)
,loader(_loader)
{
	if (!path.empty() && '/' != path[path.size() - 1] && '\\' != path[path.size() - 1])
	{
		path += '/';
	}
}

LRESULT VirtualFileLoader::Load(std::vector<BYTE>& out, const char* filename, DWORD offset, DWORD size)const
{
	if (NULL == filename)
	{
		return 1;
	}

	std::string name;
	if (::PathIsRelative(filename))
	{
		//	相対パス.
		name = GetPath();
		name += filename;

		std::vector< LoaderPair >::const_reverse_iterator it = loaders_.rbegin();
		while(loaders_.rend() != it)
		{
			const LoaderPair &pair = (*it);
			if (0 == pair.loader->Load(out, (pair.path + name).c_str(), offset, size))
			{
				return 0;
			}
			++it;
		}
	}
	else
	{
		//	絶対パス.
		name = filename;

		//	絶対パスの場合、デフォルトパスを無視する.
		std::vector< LoaderPair >::const_reverse_iterator it = loaders_.rbegin();
		while(loaders_.rend() != it)
		{
			const LoaderPair &pair = (*it);
			if (0 == pair.loader->Load(out, name.c_str(), offset, size))
			{
				return 0;
			}
			++it;
		}
	}
	return 2;
}

LRESULT VirtualFileLoader::PushLoader(SFileLoader loader, const char* path)
{
	if (NULL == loader || NULL == path)
	{
		return 1;
	}

	loaders_.push_back(LoaderPair(loader, path));
	return 0;
}

LRESULT VirtualFileLoader::PopLoader()
{
	if (loaders_.empty())
	{
		return 1;
	}

	loaders_.pop_back();
	return 0;
}
