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
		Variant(const std::string& str):asInt(atoi(str.c_str())), asFloat((float)atof(str.c_str())), asString(str){}	// コンストラクタ
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
		std::map<std::string, Variant*> value_;	// 値
		std::vector<std::string> key_;	// キー
	};

	CsvReader():maxRow_(0), maxCol_(0){}	// コンストラクタ
	CsvReader(const char* pFileName);		///
	~CsvReader();	// デストラクタ
	bool OpenFile(const char* pFileName);	// csvファイルの読み込み
	int GetMaxrow()const{ return maxRow_; }	// csvファイルの行数の取得
	int GetMaxcol()const{ return maxCol_; }	// csvファイルの列数の取得
	Variant& GetField(const char* pID, const char* pKey);	// csvファイルのフィールドを取得
	Variant& GetField(int row, const char* pKey);			///
	Variant& GetField(int row, int col);					///
	Record& operator[](const char* pID);					///
	Record& operator[](int row);							///

private:
	int maxRow_;	// 最大行数
	int maxCol_;	// 最大列数
	std::vector<std::string> id_;	// IDリスト
	std::map<std::string, Record*> record_;	// Record
	static Variant NullValue;	// 空の値
	static Record NullRecord;	// 空のRecord
};

#endif	// #ifndef INCLUDE_IDEA_CSVREADER_H