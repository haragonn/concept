/*==============================================================================
	[CsvReader.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Utility/CsvReader.h"
#include "../../h/Utility/ideaUtility.h"
#include <sstream>
#include <fstream>

using namespace std;

namespace{
	const int MAX_READ_LINE = 2048;	// 一行のデータとして読み込める最大文字数
}

// 存在しないものを指定された時に返す実体
CsvReader::Variant CsvReader::s_NullValue;
CsvReader::Record CsvReader::s_NullRecord;

// intからstringへ変換
string IntToString(int val)
{
	stringstream ss;
	ss << val;
	return ss.str();
}

// Contextクラス
// csvの読み込み方を決めるクラス
template<int N>class Context{
public:
	char str_[N];

private:
	int size_;

	int pos_;

	bool eol_;

public:
	Context() : size_(N), pos_(0),eol_(false){}

	// 項目を一つ読み進める
	string GetItem(){
		char c[N] = {};
		int j = 0;
		for(int i = pos_; i < size_; ++i, ++j, ++pos_){
			switch(str_[i]){
			// 項目の区切り
			case ',':
				++pos_;
				return c;
				break;

			// 行の終わり
			case '\0':
				++pos_;
				eol_ = true;
				return c;
				break;

			// 無視する文字
			case ' ':
			case '\t':
				--j;
				break;

			default:
				c[j] = str_[i];
			}
		}

		eol_ = true;

		return c;
	}

	// 読む位置のリセット
	void Reset(){
		pos_ = 0;

		eol_ = false;
	}

	// 一行読み終わったかどうか
	bool EOL(){
		if(eol_){
			eol_ = false;

			return true;
		}

		return false;
	}
};

// Recordクラス
// csvのデータ一列分を格納するクラス
CsvReader::Record::~Record()
{
	// mapの開放
	map<string,Variant*>::iterator it, end;

	for(it = mapValue_.begin(), end = mapValue_.end(); it != end; ++it){ delete it->second; }
}

CsvReader::Variant& CsvReader::Record::operator[](const char* pKey)
{
	// 指定されたKeyが存在すればその内容を返す
	if(mapValue_.find(pKey) == mapValue_.end()){ return s_NullValue; }

	return *mapValue_[pKey];
}

CsvReader::Variant& CsvReader::Record::operator[](int col)
{
	// 通し番号からIDを割り出してその内容を返す
	if((int)vecKey_.size() > col && col > -1){
		return *mapValue_[vecKey_[col]];
	}else{ return s_NullValue; }
}

void CsvReader::Record::Add(const char* pKey, Variant& val){
	// 指定されたKeyが存在しなければ新しく作成する
	if(mapValue_.find(pKey) == mapValue_.end()){
		mapValue_[pKey] = new Variant(val);
		vecKey_.push_back(pKey);
	}else{ (*mapValue_[pKey]) = val; }
}

// CsvReaderクラス
// 複数行のRecordからなるcsvのデータを格納するクラス
CsvReader::CsvReader(const char* pFileName)
{
	OpenFile(pFileName);
}

CsvReader::~CsvReader()
{
	// map,vectorの解放
	map<string,Record*>::iterator it, end;

	for(it = mapRecord_.begin(), end = mapRecord_.end(); it != end; ++it){
		SafeDelete(it->second);
	}

	vector<string>().swap(vecId_);
}

bool CsvReader::OpenFile(const char* pFileName)
{
	ifstream ifs(pFileName);

	if(ifs.is_open() == true){
		// データKey情報の取得
		vector<string> key;
		Context<MAX_READ_LINE> ct;
		string tStr;
		string tID;
		bool bID = false;

		do{
			ifs.getline(ct.str_, MAX_READ_LINE - 1);
			tStr = ct.str_;
		}while(tStr.empty() && !ifs.eof());
		
		colSize_ = 0;

		while(!ct.EOL()){
			key.push_back(ct.GetItem());
			++colSize_;
		}

		ct.Reset();
		
		// IDの使用確認
		if(key[0] == "id"
		|| key[0] == "Id"
		|| key[0] == "ID"){ bID = true; }

		// データ取得および格納
		rowSize_ = 0;

		while(!ifs.eof()){
			ifs.getline(ct.str_,MAX_READ_LINE - 1);
			tStr = ct.str_;

			if(!tStr.empty()){
				Record* tRecord = new Record();

				// IDの使用確認
				if(bID){
					tID = ct.GetItem();
					vecId_.push_back(tID);
				}else{
					tID = IntToString(rowSize_);
					vecId_.push_back(tID);
				}
				for(int i = 0;i < colSize_ && !ct.EOL();++i){
					if(i == 0 && bID){ continue; }

					Variant val = Variant(ct.GetItem());
					tRecord->Add(key[i].c_str(), val);
				}

				mapRecord_[tID] = tRecord;

				++rowSize_;
			}
			ct.Reset();
		}
		return true;
	}

	return false;
}

CsvReader::Variant& CsvReader::GetField(const char* pID, const char* pKey)
{
	// 指定されたIDが存在すればその内容を返す,Keyの判定はoperator[](pKey)に任せる
	if(mapRecord_.find(pID) == mapRecord_.end()){ return s_NullValue; }

	return mapRecord_[pID]->operator[](pKey);
}

CsvReader::Variant& CsvReader::GetField(int row, const char* pKey)
{
	// 通し番号からIDを割り出す
	if((int)vecId_.size() > row && row >= 0){
		return mapRecord_[vecId_[row]]->operator[](pKey);	// 指定されたIDの内容を返す,pKeyの判定はoperator[](pKey)に任せる
	}else{ return s_NullValue; }
}

CsvReader::Variant& CsvReader::GetField(int row, int col)
{
	// 通し番号からIDを割り出す
	if((int)vecId_.size() > row && row >= 0){
		return mapRecord_[vecId_[row]]->operator[](col);	// 指定されたID,Keyの内容を返す
	}else{ return s_NullValue; }
}

CsvReader::Record& CsvReader::operator[](const char* pID)
{
	// 指定されたIDが存在すればその内容を返す
	if(mapRecord_.find(pID) == mapRecord_.end()){ return s_NullRecord; }

	return *mapRecord_[pID];
}

CsvReader::Record& CsvReader::operator[](int row)
{
	// 通し番号からIDを割り出してその内容を返す
	if((int)vecId_.size() > row && row > -1){
		return *mapRecord_[vecId_[row]];
	}else{ return s_NullRecord; }
}