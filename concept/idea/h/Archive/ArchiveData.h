#pragma once

#pragma warning (disable:4786)
#include "IFileLoader.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <map>

class ArchiveData : public IFileLoader
{

protected:
	typedef struct
	{
		DWORD dwIndex;		//	ファイル番号.
		DWORD dwFileOffset;	//	ファイル先頭オフセット値.
		DWORD dwFileSize;	//	ファイルサイズ.
	}
	FileHeader;

	typedef std::map<std::string, FileHeader> FileHeaderList;

	std::string	archiveFileName_;	//	マウントしたアーカイブファイル. 絶対パス.
	FileHeaderList headers_;

	ArchiveData(){}

public:
	static ArchiveData* CreateInstance(const char* archiveFileName);
	virtual LRESULT Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;
};
