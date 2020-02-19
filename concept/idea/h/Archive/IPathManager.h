#pragma once
#include <windows.h>
#include <string>
#include <memory>

class IPathManager
{
public:
	/**
	デストラクタ.
	*/
	virtual ~IPathManager(){};

	/**
	スタックにパスを登録する.
	絶対パスを登録した場合は、カレントパスは絶対パスとなる.
	相対パスを登録した場合は、カレントパスは登録前のカレントパスに相対パスを適用したパスとなる.
	@param	path	登録するパス.
	@return			エラーコード.
	*/
	virtual LRESULT PushPath(const char* path) = 0;

	/**
	スタックから最新の登録パスを取り除く.
	最後に成功したpushPath(const WCHAR *path)の影響を取り除く.
	@return			エラーコード. スタックに1つもパスが無い場合に呼び出された場合、エラーとなる.
	*/
	virtual LRESULT PopPath() = 0;

	/**
	現在のカレントパスを取得する.
	@return			現在のカレントパス文字列へのポインタ.
	*/
	virtual const char* GetPath()const = 0;

	/**
	パスを正規化する.
	'/'を'\\'に変換した上で、不要な".", ".."を含まないパスに変換される.
	エラーの場合はpathに変化はおきない.
	@param	path	正規化するパス文字列. メソッドが正解した場合のみ、正規化されたパス文字列に書き変わる.
	@return			エラーコード.
	*/
	static LRESULT NormalizePath(std::string& path);
};

typedef std::shared_ptr<IPathManager> SPathManager;