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

	//	相対パスならば、絶対パスへ変更.
	if (false && ::PathIsRelative(archiveFileName))
	{
		DWORD size = ::GetCurrentDirectoryW(0, NULL);
		char* p = new char [size];
		::GetCurrentDirectory(size, p);
		name = string(p);
		delete [] p;

		//	末尾チェックを追加.
		//	"c:"など、ルートディレクトリでGetCurrentDirectoryを使った場合のみ、末尾に既に'\\'がついているため.
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

	//	ヘッダ読み込み.
	FileLoader loader;
	if (0 != loader.Load(buffer, archiveFileName, 0, 8))
	{
		return NULL;
	}

	//	マジックナンバーをチェック.
	if ('\0' != buffer[0] || 'I' != buffer[1] || 'A' != buffer[2] || 'F' != buffer[3])
	{
		return NULL;
	}

	int headerSize;
	headerSize = *(int*)&buffer[4];

	//	ヘッダー部(から、マジックナンバーとヘッダーサイズ領域を除いた部分)を読み込む.
	if (0 != loader.Load(buffer, archiveFileName, 8, headerSize - 8))
	{
		return NULL;
	}

	//	解析.
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

	//	ファイルヘッダのリストを作成する.
	FileHeaderList headers;

	//	ファイル先頭へのオフセット値.
	//	初期値はヘッダーサイズ分.
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

	//	パス名の正規化.
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

	//	絶対パスではロードできない.
	if (!::PathIsRelative(name.c_str()))
	{
		return 2;
	}

	//	ファイル名で検索.
	FileHeaderList::const_iterator it = headers_.find(name);
	if (headers_.end() == it)
	{
		return 3;
	}

	//	検索にヒットしたファイルのヘッダ情報を参照.
	const FileHeader &header = it->second;

	//	オフセットがファイルサイズ以上ならエラー.
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

	//	正常にデータが取得できたので、swapを使いoutと配列参照を交換.
	out.swap(buffer);
	return 0;
}
