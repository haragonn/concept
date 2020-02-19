/*==============================================================================
	[IniReader.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Utility/IniReader.h"
#include <stdio.h>
#include <Windows.h>

BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int value, LPCTSTR lpFileName)
{
	char buf[16];

	sprintf_s(buf, sizeof(buf), "%d", value);

	return WritePrivateProfileString(lpAppName, lpKeyName, buf, lpFileName);
}

IniReader::IniReader()
{
	ZeroMemory(fullPath_, sizeof(fullPath_));
}

IniReader::IniReader(const char* pFileName, bool bCreate)
{
	OpenFile(pFileName, bCreate);
}

bool IniReader::OpenFile(const char* pFileName, bool bCreate)
{
	FILE* fp;

	if(fopen_s(&fp, pFileName, "r") != 0){
		if(bCreate){
			fopen_s(&fp, pFileName, "w");
			_fullpath(fullPath_, pFileName, 260);
			fclose(fp);
		}
		return false;
	}

	_fullpath(fullPath_, pFileName, 260);
	fclose(fp);

	return true;
}

int IniReader::ReadInt(const char* pSection, const char* pKey, int def)
{
	if(!fullPath_[0]){ return def; }

	return GetPrivateProfileInt(pSection, pKey, def, fullPath_);
}

int IniReader::ReadString(const char* pSection, const char* pKey, char* pBuffer, int size, const char* def)
{
	if(!fullPath_[0]){ return 0; }

	return GetPrivateProfileString(pSection, pKey, def, pBuffer, size, fullPath_);
}

bool IniReader::WriteInt(const char* pSection, const char* pKey, int value)
{
	if(!fullPath_[0]){ return false; }

	return !!WritePrivateProfileInt(pSection, pKey, value, fullPath_);
}

bool IniReader::WriteString(const char* pSection, const char* pKey, char* pString)
{
	if(!fullPath_[0]){ return false; }
	
	return !!WritePrivateProfileString(pSection, pKey, pString, fullPath_);
}