/*==============================================================================
	[CsvReader.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Utility/CsvReader.h"
#include "../../h/Utility/ideaUtility.h"
#include <sstream>
#include <fstream>

namespace{
	const int MAX_READ_LINE = 2048;	// 一行のデータとして読み込める最大文字数
}

// 存在しないものを指定された時に返す実体
CsvReader::Variant CsvReader::NullValue;
CsvReader::Record CsvReader::NullRecord;

// intからstringへ変換
std::string IntToString(int val)
{
	std::stringstream ss;
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
	std::string GetItem(){
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
	std::map<std::string,Variant*>::iterator it, end;
	for(it = value_.begin(), end = value_.end(); it != end; ++it){ delete it->second; }
}

CsvReader::Variant& CsvReader::Record::operator[](const char* pKey)
{
	// 指定されたKeyが存在すればその内容を返す
	if(value_.find(pKey) == value_.end()){ return NullValue; }
	return *value_[pKey];
}

CsvReader::Variant& CsvReader::Record::operator[](int col)
{
	// 通し番号からIDを割り出してその内容を返す
	if((int)key_.size() > col && col > -1){
		return *value_[key_[col]];
	}else{ return NullValue; }
}

void CsvReader::Record::Add(const char* pKey, Variant& val){
	// 指定されたKeyが存在しなければ新しく作成する
	if(value_.find(pKey) == value_.end()){
		value_[pKey] = new Variant(val);
		key_.push_back(pKey);
	}
	else{ (*value_[pKey]) = val; }
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
	std::map<std::string,Record*>::iterator it, end;
	for(it = record_.begin(), end = record_.end(); it != end; ++it){ delete it->second; }
	std::vector<std::string>().swap(id_);
}

bool CsvReader::OpenFile(const char* pFileName)
{
	std::ifstream ifs(pFileName);

	if(ifs.is_open() == true){
		// データKey情報の取得
		std::vector<std::string> key;
		Context<MAX_READ_LINE> ct;
		std::string tStr;
		std::string tID;
		bool bID = false;

		do{
			ifs.getline(ct.str_, MAX_READ_LINE - 1);
			tStr = ct.str_;
		}while(tStr.empty() && !ifs.eof());
		
		maxCol_ = 0;
		while(!ct.EOL()){
			key.push_back(ct.GetItem());
			++maxCol_;
		}
		ct.Reset();
		
		// IDの使用確認
		if(key[0] == "id"
		|| key[0] == "Id"
		|| key[0] == "ID"){ bID = true; }

		// データ取得および格納
		maxRow_ = 0;
		while(!ifs.eof()){
			ifs.getline(ct.str_,MAX_READ_LINE - 1);
			tStr = ct.str_;
			if(!tStr.empty()){
				Record* tRecord = new Record();
				// IDの使用確認
				if(bID){
					tID = ct.GetItem();
					id_.push_back(tID);
				}else{
					tID = IntToString(maxRow_);
					id_.push_back(tID);
				}
				for(int i = 0;i < maxCol_ && !ct.EOL();++i){
					if(i == 0 && bID){ continue; }
					Variant val = Variant(ct.GetItem());
					tRecord->Add(key[i].c_str(), val);
				}
				record_[tID] = tRecord;
				++maxRow_;
			}
			ct.Reset();
		}
		return true;
	}// if(ifs.is_open() == true)
	return false;
}

CsvReader::Variant& CsvReader::GetField(const char* pID, const char* pKey)
{
	// 指定されたIDが存在すればその内容を返す,Keyの判定はoperator[](pKey)に任せる
	if(record_.find(pID) == record_.end()){ return NullValue; }
	return record_[pID]->operator[](pKey);
}

CsvReader::Variant& CsvReader::GetField(int row, const char* pKey)
{
	// 通し番号からIDを割り出す
	if((int)id_.size() > row && row >= 0){
		// 指定されたIDの内容を返す,pKeyの判定はoperator[](pKey)に任せる
		return record_[id_[row]]->operator[](pKey);
	}else{ return NullValue; }
}

CsvReader::Variant& CsvReader::GetField(int row, int col)
{
	// 通し番号からIDを割り出す
	if((int)id_.size() > row && row >= 0){
		// 指定されたID,Keyの内容を返す
		return record_[id_[row]]->operator[](col);
	}else{ return NullValue; }
}

CsvReader::Record& CsvReader::operator[](const char* pID)
{
	// 指定されたIDが存在すればその内容を返す
	if(record_.find(pID) == record_.end()){ return NullRecord; }
	return *record_[pID];
}

CsvReader::Record& CsvReader::operator[](int row)
{
	// 通し番号からIDを割り出してその内容を返す
	if((int)id_.size() > row && row > -1){
		return *record_[id_[row]];
	}else{ return NullRecord; }
}