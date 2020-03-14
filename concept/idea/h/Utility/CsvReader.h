/*==============================================================================
	[CsvReader.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_CSVREADER_H
#define INCLUDE_IDEA_CSVREADER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <map>

//------------------------------------------------------------------------------
// クラス名　：CsvReader
// クラス概要：csvファイルの読み込みを行うクラス
//------------------------------------------------------------------------------
class CsvReader{
public:
	// Variant構造体
	// 複数の型として扱える構造体
	struct Variant{
		int asInt;				// intとしての値
		float asFloat;			// floatとしての値
		std::string asString;	// std::stringとしての値

		Variant():asInt(0),asFloat(0){}	// コンストラクタ
		Variant(const std::string& str) : asInt(atoi(str.c_str())), asFloat((float)atof(str.c_str())), asString(str){}	// コンストラクタ

		operator int(){ return asInt; }				// intとして受け取った場合
		operator float(){ return asFloat; }			// floatとして受け取った場合
		operator std::string(){ return asString; }	// stringとして受け取った場合
	};

	// Recordクラス
	// csvのデータ一列分を格納するクラス
	class Record{
	public:
		~Record();	// デストラクタ

		Variant& operator[](const char* pKey);	// operator[]
		Variant& operator[](int col);			///

		void Add(const char* pKey, Variant& val);	// 値の追加

	private:
		std::map<std::string, Variant*> mapValue_;	// 値
		std::vector<std::string> vecKey_;	// キー
	};

	CsvReader():rowSize_(0), colSize_(0){}	// コンストラクタ
	CsvReader(const char* pFileName);		///

	~CsvReader();	// デストラクタ

	bool OpenFile(const char* pFileName);	// csvファイルの読み込み

	int GetRowSize()const{ return rowSize_; }	// csvファイルの行数の取得
	int GetColSize()const{ return colSize_; }	// csvファイルの列数の取得

	Variant& GetField(const char* pID, const char* pKey);	// csvファイルのフィールドを取得
	Variant& GetField(int row, const char* pKey);			///
	Variant& GetField(int row, int col);					///

	Record& operator[](const char* pID);					///
	Record& operator[](int row);							///

private:
	int rowSize_;	// 行数
	int colSize_;	// 列数

	std::vector<std::string> vecId_;	// IDリスト
	std::map<std::string, Record*> mapRecord_;	// Record

	static Variant s_NullValue;	// 空の値
	static Record s_NullRecord;	// 空のRecord
};

#endif	// #ifndef INCLUDE_IDEA_CSVREADER_H