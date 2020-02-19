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
		DWORD dwIndex;		//	�t�@�C���ԍ�.
		DWORD dwFileOffset;	//	�t�@�C���擪�I�t�Z�b�g�l.
		DWORD dwFileSize;	//	�t�@�C���T�C�Y.
	}
	FileHeader;

	typedef std::map<std::string, FileHeader> FileHeaderList;

	std::string	archiveFileName_;	//	�}�E���g�����A�[�J�C�u�t�@�C��. ��΃p�X.
	FileHeaderList headers_;

	ArchiveData(){}

public:
	static ArchiveData* CreateInstance(const char* archiveFileName);
	virtual LRESULT Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;
};
