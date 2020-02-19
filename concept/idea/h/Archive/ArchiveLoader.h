#ifndef INCLUDE_IDEA_ARCHIVELOADER_H
#define INCLUDE_IDEA_ARCHIVELOADER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>

class ArchiveLoader{
public:
	ArchiveLoader(){ std::vector<BYTE>().swap(data_); }
	~ArchiveLoader(){ std::vector<BYTE>().swap(data_); }
	bool Load(const char* archiveFileName, const char* fileName);
	std::vector<BYTE>& GetData(){ return data_; }
	size_t GetSize(){ return data_.size(); }

private:
	std::vector<BYTE> data_;

	// コピーコンストラクタの禁止
	ArchiveLoader(const ArchiveLoader& src){}
	ArchiveLoader& operator=(const ArchiveLoader& src){}
};

#endif	// #ifndef INCLUDE_IDEA_ARCHIVELOADER_H