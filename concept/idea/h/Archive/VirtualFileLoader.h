#pragma once

#include "IFileLoader.h"
#include "PathManager.h"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/**
複数フォルダに散っているファイルを、1つのフォルダ内に存在するかのように仮想的に扱えるファイルローダ.
パス管理とロード機能も持つ.
*/
class VirtualFileLoader : public IFileLoader, public PathManager
{
public:
	struct LoaderPair
	{
		SFileLoader loader;	//	ローダー.
		std::string	path;	//	基点パス名.

		LoaderPair(SFileLoader loader, std::string path);
	};

	//	スタックとしてもつ.
	std::vector<LoaderPair> loaders_;

public:
	/**
	新しいファイルローダを登録する. スタックとして積まれ、後から積んだものが優先して利用される.
	loaderで渡したインスタンスは、popLoader()が呼ばれた時と、CVirtualFileLoaderのインスタンスが破壊された時に削除される.
	@param	loader	ファイルローダのインスタンス.
	@param	path	基点パス名.
	@return			エラーコード. loaderがNULLの場合にエラーとなる.
	*/
	virtual LRESULT PushLoader(SFileLoader loader, const char *path);

	/**
	新しいファイルローダを登録する. スタックとして積まれ、後から積んだものが優先して利用される.
	loaderで渡したインスタンスは、popLoader()が呼ばれた時と、CVirtualFileLoaderのインスタンスが破壊された時に削除される.
	@param	loader	ファイルローダのインスタンス.
	@param	path	基点パス名.
	@return			エラーコード. loaderがNULLの場合にエラーとなる.
	*/
	//virtual LRESULT pushLoader(SFileLoader loader, const WCHAR *path);

	/**
	最後に登録されたファイルローダをスタックから下ろす.
	@return			エラーコード. ファイルローダが1つも積まれていない時に呼び出された場合にエラーとなる.
	*/
	virtual LRESULT PopLoader();

	/**
	ファイルをロードする.
	その際、後から登録されたファイルローダを優先的に使う.
	ロードに失敗した場合、次に登録されているファイルローダでロードを試みるという動作を繰り返す.
	全てのファイルローダでロードが失敗した場合にエラーを返す.
	@param	out			ロードしたデータを格納する配列への参照. ロードに失敗した場合は変化は無い.
	@param	filename	ファイル名.
	@param	offset		読み込み開始位置オフセット. ファイルサイズよりも大きい値が指定されていた場合、エラーとなる.
	@param	size		読み込むサイズ制限. 0を指定した場合はファイルの終端まで読み込む.
	@return				エラーコード.
	*/
	virtual LRESULT Load(std::vector<BYTE>& out, const char* filename, DWORD offset = 0, DWORD size = 0)const;	
};