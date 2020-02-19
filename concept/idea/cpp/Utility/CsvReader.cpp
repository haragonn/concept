/*==============================================================================
	[CsvReader.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Utility/CsvReader.h"
#include "../../h/Utility/ideaUtility.h"
#include <sstream>
#include <fstream>

namespace{
	const int MAX_READ_LINE = 2048;	// ��s�̃f�[�^�Ƃ��ēǂݍ��߂�ő啶����
}

// ���݂��Ȃ����̂��w�肳�ꂽ���ɕԂ�����
CsvReader::Variant CsvReader::NullValue;
CsvReader::Record CsvReader::NullRecord;

// int����string�֕ϊ�
std::string IntToString(int val)
{
	std::stringstream ss;
	ss << val;
	return ss.str();
}

// Context�N���X
// csv�̓ǂݍ��ݕ������߂�N���X
template<int N>class Context{
public:
	char str_[N];
private:
	int size_;
	int pos_;
	bool eol_;
public:
	Context() : size_(N), pos_(0),eol_(false){}
	// ���ڂ���ǂݐi�߂�
	std::string GetItem(){
		char c[N] = {};
		int j = 0;
		for(int i = pos_; i < size_; ++i, ++j, ++pos_){
			switch(str_[i]){
			// ���ڂ̋�؂�
			case ',':
				++pos_;
				return c;
				break;
			// �s�̏I���
			case '\0':
				++pos_;
				eol_ = true;
				return c;
				break;
			// �������镶��
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
	// �ǂވʒu�̃��Z�b�g
	void Reset(){
		pos_ = 0;
		eol_ = false;
	}
	// ��s�ǂݏI��������ǂ���
	bool EOL(){
		if(eol_){
			eol_ = false;
			return true;
		}
		return false;
	}
};

// Record�N���X
// csv�̃f�[�^��񕪂��i�[����N���X
CsvReader::Record::~Record()
{
	// map�̊J��
	std::map<std::string,Variant*>::iterator it, end;
	for(it = value_.begin(), end = value_.end(); it != end; ++it){ delete it->second; }
}

CsvReader::Variant& CsvReader::Record::operator[](const char* pKey)
{
	// �w�肳�ꂽKey�����݂���΂��̓��e��Ԃ�
	if(value_.find(pKey) == value_.end()){ return NullValue; }
	return *value_[pKey];
}

CsvReader::Variant& CsvReader::Record::operator[](int col)
{
	// �ʂ��ԍ�����ID������o���Ă��̓��e��Ԃ�
	if((int)key_.size() > col && col > -1){
		return *value_[key_[col]];
	}else{ return NullValue; }
}

void CsvReader::Record::Add(const char* pKey, Variant& val){
	// �w�肳�ꂽKey�����݂��Ȃ���ΐV�����쐬����
	if(value_.find(pKey) == value_.end()){
		value_[pKey] = new Variant(val);
		key_.push_back(pKey);
	}
	else{ (*value_[pKey]) = val; }
}

// CsvReader�N���X
// �����s��Record����Ȃ�csv�̃f�[�^���i�[����N���X
CsvReader::CsvReader(const char* pFileName)
{
	OpenFile(pFileName);
}

CsvReader::~CsvReader()
{
	// map,vector�̉��
	std::map<std::string,Record*>::iterator it, end;
	for(it = record_.begin(), end = record_.end(); it != end; ++it){ delete it->second; }
	std::vector<std::string>().swap(id_);
}

bool CsvReader::OpenFile(const char* pFileName)
{
	std::ifstream ifs(pFileName);

	if(ifs.is_open() == true){
		// �f�[�^Key���̎擾
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
		
		// ID�̎g�p�m�F
		if(key[0] == "id"
		|| key[0] == "Id"
		|| key[0] == "ID"){ bID = true; }

		// �f�[�^�擾����ъi�[
		maxRow_ = 0;
		while(!ifs.eof()){
			ifs.getline(ct.str_,MAX_READ_LINE - 1);
			tStr = ct.str_;
			if(!tStr.empty()){
				Record* tRecord = new Record();
				// ID�̎g�p�m�F
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
	// �w�肳�ꂽID�����݂���΂��̓��e��Ԃ�,Key�̔����operator[](pKey)�ɔC����
	if(record_.find(pID) == record_.end()){ return NullValue; }
	return record_[pID]->operator[](pKey);
}

CsvReader::Variant& CsvReader::GetField(int row, const char* pKey)
{
	// �ʂ��ԍ�����ID������o��
	if((int)id_.size() > row && row >= 0){
		// �w�肳�ꂽID�̓��e��Ԃ�,pKey�̔����operator[](pKey)�ɔC����
		return record_[id_[row]]->operator[](pKey);
	}else{ return NullValue; }
}

CsvReader::Variant& CsvReader::GetField(int row, int col)
{
	// �ʂ��ԍ�����ID������o��
	if((int)id_.size() > row && row >= 0){
		// �w�肳�ꂽID,Key�̓��e��Ԃ�
		return record_[id_[row]]->operator[](col);
	}else{ return NullValue; }
}

CsvReader::Record& CsvReader::operator[](const char* pID)
{
	// �w�肳�ꂽID�����݂���΂��̓��e��Ԃ�
	if(record_.find(pID) == record_.end()){ return NullRecord; }
	return *record_[pID];
}

CsvReader::Record& CsvReader::operator[](int row)
{
	// �ʂ��ԍ�����ID������o���Ă��̓��e��Ԃ�
	if((int)id_.size() > row && row > -1){
		return *record_[id_[row]];
	}else{ return NullRecord; }
}