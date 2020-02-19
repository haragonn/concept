#include "../../h/Archive/ArchiveData.h"
#include "../../h/Archive/FileLoader.h"
#include <vector>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define _CRT_DISABLE_PERFCRIT_LOCKS

using namespace std;

ArchiveData *ArchiveData::CreateInstance(const char* archiveFileName)
{
	string name;

	//	���΃p�X�Ȃ�΁A��΃p�X�֕ύX.
	if (false && ::PathIsRelative(archiveFileName))
	{
		DWORD size = ::GetCurrentDirectoryW(0, NULL);
		char* p = new char [size];
		::GetCurrentDirectory(size, p);
		name = string(p);
		delete [] p;

		//	�����`�F�b�N��ǉ�.
		//	"c:"�ȂǁA���[�g�f�B���N�g����GetCurrentDirectory���g�����ꍇ�̂݁A�����Ɋ���'\\'�����Ă��邽��.
		if (!name.empty() && '\\' != name[name.size() - 1])
		{
			name += '\\';
		}
		name += archiveFileName;
	}
	else
	{
		name = archiveFileName;
	}

	vector<byte> buffer;

	//	�w�b�_�ǂݍ���.
	FileLoader loader;
	if (0 != loader.Load(buffer, archiveFileName, 0, 8))
	{
		return NULL;
	}

	//	�}�W�b�N�i���o�[���`�F�b�N.
	if ('\0' != buffer[0] || 'I' != buffer[1] || 'A' != buffer[2] || 'F' != buffer[3])
	{
		return NULL;
	}

	int headerSize;
	headerSize = *(int*)&buffer[4];

	//	�w�b�_�[��(����A�}�W�b�N�i���o�[�ƃw�b�_�[�T�C�Y�̈������������)��ǂݍ���.
	if (0 != loader.Load(buffer, archiveFileName, 8, headerSize - 8))
	{
		return NULL;
	}

	//	���.
	int fileNum = *(int*)&buffer[0];
	vector<string>	filePaths;
	vector<int>		fileSizes;
	filePaths.reserve(fileNum);
	fileSizes.reserve(fileNum);

	DWORD i = 4;
	for(int n = 0; n < fileNum; ++n)
	{
		int length = *(int*)&buffer[i];
		i += sizeof(int);

		string fileName = string((const char*)&buffer[i], 0, length / sizeof(char));
		filePaths.push_back(fileName);
		i += length;

		fileSizes.push_back(*(int*)&buffer[i]);
		i += sizeof(int);
	}

	//	�t�@�C���w�b�_�̃��X�g���쐬����.
	FileHeaderList headers;

	//	�t�@�C���擪�ւ̃I�t�Z�b�g�l.
	//	�����l�̓w�b�_�[�T�C�Y��.
	DWORD offset = headerSize;
	for(int n = 0; n < fileNum; ++n)
	{
		FileHeader h;
		h.dwIndex = n;
		h.dwFileOffset = offset;
		h.dwFileSize = fileSizes[n];

		headers.insert(pair<string, FileHeader>(filePaths[n], h));

		offset += fileSizes[n];
	}

	ArchiveData* p = new ArchiveData;
	p->archiveFileName_ = name;
	p->headers_ = headers;
	return p;
}

LRESULT ArchiveData::Load(vector<byte>& out, const char* filename, DWORD offset, DWORD size)const
{
	vector<byte>().swap(out);
	string name = filename;

	//	�p�X���̐��K��.
	{
		int n = 0;
		while(-1 != (n = (int)name.find('/', n)))
		{
			name = name.replace(n, 1, "\\");
		}
		vector<char> p(name.size() + 1);
		if (!::PathCanonicalize(&p[0], name.c_str()))
		{
			return 1;
		}
		name = string(&p[0]);
	}

	//	��΃p�X�ł̓��[�h�ł��Ȃ�.
	if (!::PathIsRelative(name.c_str()))
	{
		return 2;
	}

	//	�t�@�C�����Ō���.
	FileHeaderList::const_iterator it = headers_.find(name);
	if (headers_.end() == it)
	{
		return 3;
	}

	//	�����Ƀq�b�g�����t�@�C���̃w�b�_�����Q��.
	const FileHeader &header = it->second;

	//	�I�t�Z�b�g���t�@�C���T�C�Y�ȏ�Ȃ�G���[.
	if (header.dwFileSize < offset)
	{
		return 4;
	}

	if (0 == size)
	{
		size = (header.dwFileSize - offset);
	}
	else if ((header.dwFileSize - offset) < size)
	{
		size = (header.dwFileSize - offset);
	}

	vector<byte> buffer;
	if (0 < header.dwFileSize)
	{
		FileLoader loader;
		if (0 != loader.Load(buffer, archiveFileName_.c_str(), header.dwFileOffset + offset, size))
		{
			return 5;
		}
	}
	else
	{
		buffer.resize(0);
	}

	//	����Ƀf�[�^���擾�ł����̂ŁAswap���g��out�Ɣz��Q�Ƃ�����.
	out.swap(buffer);
	return 0;
}
