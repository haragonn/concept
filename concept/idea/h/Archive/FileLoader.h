#pragma once

#include "IFileLoader.h"

class FileLoader : public IFileLoader
{
public:
	virtual LRESULT Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;
};
