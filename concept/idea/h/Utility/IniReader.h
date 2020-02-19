/*==============================================================================
	[IniReader.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_INIREADER_H
#define INCLUDE_IDEA_INIREADER_H

//------------------------------------------------------------------------------
// クラス名　：IniReader
// クラス概要：iniファイルの読み書きを行うクラス
//------------------------------------------------------------------------------
class IniReader{
public:
	IniReader();											// コンストラクタ
	IniReader(const char* pFileName, bool bCreate = true);	//
	bool OpenFile(const char* pFileName, bool bCreate = true);	// iniファイルを開く
	int ReadInt(const char* pSection, const char* pKey, int def = 0);	// 整数の読み込み
	int ReadString(const char* pSection, const char* pKey, char* pBuffer, int size, const char* def = "");	// 文字列の読み込み
	bool WriteInt(const char* pSection, const char* pKey, int value);			// 整数の書き込み
	bool WriteString(const char* pSection, const char* pKey, char* pString);	// 文字列の書き込み

private:
	char fullPath_[260];	// ファイルパス
};

#endif	// #ifndef INCLUDE_IDEA_INIREADER_H