#pragma once

#include "IPathManager.h"
#include <vector>
#include <string>

class PathManager : public IPathManager
{
	std::vector<std::string> paths_;
public:
	/**
	スタックにパスを登録する.
	絶対パスを登録した場合は、カレントパスは絶対パスとなる.
	相対パスを登録した場合は、カレントパスは登録前のカレントパスに相対パスを適用したパスとなる.
	@param	path	登録するパス.
	@return			エラーコード.
	*/
	virtual LRESULT PushPath(const char* path);

	/**
	スタックから最新の登録パスを取り除く.
	最後に成功したpushPath(const WCHAR *path)の影響を取り除く.
	@return			エラーコード. スタックに1つもパスが無い場合に呼び出された場合、エラーとなる.
	*/
	virtual LRESULT PopPath();

	/**
	現在のカレントパスを取得する.
	@return			現在のカレントパス文字列へのポインタ.
	*/
	virtual const char* GetPath()const;
};
