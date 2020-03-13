/*==============================================================================
	[ideaUtility.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_IDEAUTILITY_H
#define INCLUDE_IDEA_IDEAUTILITY_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <string>
#include <locale>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#define SetDebugMessage(str,...){ TCHAR c[1024];_stprintf_s(c,(sizeof(c)/sizeof(c[0])),_T(str),__VA_ARGS__);OutputDebugString(c); }
#define SetLeakCheckFlag(){ _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); }
#define GetHeapSize(){ SetDebugMessage("/*==============================================================================\n\t\t%s(%d)\n================================================================================\n",__FILE__,__LINE__);_CrtDumpMemoryLeaks();SetDebugMessage("=================================================================================\n\t\t%s(%d)\n===============================================================================*/\n",__FILE__,__LINE__); }
#define Assert(exp){ _ASSERT(exp); }
#else
#define SetDebugMessage(str,...)
#define SetLeakCheckFlag()
#define GetHeapSize()
#define Assert(exp)
#endif

template <typename T>
inline void SafeDelete(T*& p)
{
	if(p){
		delete p;
		p = nullptr;
	}
}

template <typename T>
inline void SafeDeleteArray(T*& p)
{
	if(p) {
		delete[] p;
		p = nullptr;
	}
}

template <typename T>
inline void SafeRelease(T*& p)
{
	if(p) {
		p->Release();
		p = nullptr;
	}
}

template <typename T>
inline void SafeDestroy(T*& p)
{
	if(p) {
		p->Destroy();
		p = nullptr;
	}
}

template <typename T>
inline void SafeUnLoad(T*& p)
{
	if(p) {
		p->UnLoad();
		p = nullptr;
	}
}

template <typename T, size_t SIZE>
size_t ArraySize(const T(&)[SIZE])
{
	return SIZE;
}

inline void AddDirectoryPath(std::string& fileName, std::string& src)
{
	int n = src.rfind("/");

	if(n < 0)
	{
		const char* t = src.c_str();

		for(int i = src.size() - 1; i >= 1; --i)
		{
			if(t[i] == '\\'){
				fileName = src.substr(0, i - 1) + fileName;

				break;
			}
		}
	} else{
		fileName = src.substr(0, n + 1) + fileName;
	}
}

inline std::string WStringToString(std::wstring wstring)
{
	setlocale(LC_ALL, "Japanese");

	int bufferSize = WideCharToMultiByte(CP_OEMCP, 0, wstring.c_str(), -1, (char*)NULL, 0, NULL, NULL);

	CHAR* pMultiByte = new CHAR[bufferSize];

	WideCharToMultiByte(CP_OEMCP, 0, wstring.c_str(), -1, pMultiByte, bufferSize, NULL, NULL);

	std::string ret(pMultiByte, pMultiByte + bufferSize - 1);

	SafeDeleteArray(pMultiByte);

	return ret;
}

inline std::wstring StringToWString(std::string string)
{
	setlocale(LC_ALL, "Japanese");

	int bufferSize = MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, (wchar_t*)NULL, 0);

	wchar_t* pUCS2 = new wchar_t[bufferSize];

	MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, pUCS2, bufferSize);

	std::wstring ret(pUCS2, pUCS2 + bufferSize - 1);

	SafeDeleteArray(pUCS2);

	return ret;
}

#endif	// #ifndef INCLUDE_IDEA_IDEAUTILITY_H